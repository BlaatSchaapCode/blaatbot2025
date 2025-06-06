#include "GnuTlsConnection.hpp"

#include "utils/logger.hpp"

namespace geblaat {
GnuTlsConnection::GnuTlsConnection() {
    gnutls_global_init();
    gnutls_certificate_allocate_credentials(&xcred);
    gnutls_certificate_set_x509_system_trust(xcred);
}

GnuTlsConnection::~GnuTlsConnection() {
    onDisconnected();

    LOG_INFO("Stopping receive thread ");
    m_receiveThreadActive = false;
    LOG_INFO("Closing socket");

    if (m_receiveThread->joinable())
        m_receiveThread->join();
    LOG_INFO("Deleting Receive Thread", 0);
    delete m_receiveThread;

    gnutls_certificate_free_credentials(xcred);
    gnutls_global_deinit();
}

int GnuTlsConnection::connect() {
    gnutls_protocol_set_enabled(GNUTLS_SSL3, ignoreInsecureProtocol);
    gnutls_protocol_set_enabled(GNUTLS_TLS1_0, ignoreInsecureProtocol);
    gnutls_protocol_set_enabled(GNUTLS_TLS1_1, ignoreInsecureProtocol);
    gnutls_protocol_set_enabled(GNUTLS_TLS1_2, true);
    gnutls_protocol_set_enabled(GNUTLS_TLS1_3, true);

    gnutls_init(&session, GNUTLS_CLIENT);

    if (!ignoreInvalidCerficiate) {
        gnutls_server_name_set(session, GNUTLS_NAME_DNS, mHostName.c_str(), mHostName.length());
        gnutls_session_set_verify_cert(session, mHostName.c_str(), 0);
    }
    gnutls_credentials_set(session, GNUTLS_CRD_CERTIFICATE, xcred);

    gnutls_set_default_priority(session);

    return TcpConnection::connect();
}

void GnuTlsConnection::onConnected() {
    gnutls_transport_set_int(session, m_socket);
    gnutls_handshake_set_timeout(session, GNUTLS_DEFAULT_HANDSHAKE_TIMEOUT);

    int ret;
    do {
        ret = gnutls_handshake(session);
    } while (ret < 0 && gnutls_error_is_fatal(ret) == 0);

    if (ret < 0) {
        if (gnutls_error_is_fatal(ret)) {
            LOG_ERROR("*** Handshake failed: Fatal error: %s", gnutls_strerror(ret));
            onDisconnected();
            return;
        } else {
            // not fatal error
            LOG_ERROR("*** Non-fatal error: %s", gnutls_strerror(ret));

            if (!ignoreInvalidCerficiate) {
                LOG_ERROR("Aborting on non fatal (certificate?) error");
                onDisconnected();
                return;
            } else {
                LOG_INFO("Ignoring certificate error");
            }
        }
    }

    LOG_INFO("- Handshake was completed\n");

    LOG_INFO("   protocol: %s", gnutls_protocol_get_name(gnutls_protocol_get_version(session)));
    LOG_INFO("   cipher:   %s",
             gnutls_cipher_suite_get_name(gnutls_kx_get(session), gnutls_cipher_get(session), gnutls_mac_get(session)));

    m_receiveThreadActive = true;
    m_receiveThread = new std::thread(GnuTlsConnection::receiveThreadFunc, this);
    mProtocol->onConnected();
}

void GnuTlsConnection::onDisconnected() {
    gnutls_bye(session, GNUTLS_SHUT_RDWR);
    gnutls_deinit(session);
    closesocket(m_socket);
    mProtocol->onDisconnected();
}

void GnuTlsConnection::send(std::vector<char> data) {
    int bytes_sent = gnutls_record_send(session, data.data(), data.size());
    if (bytes_sent == (int)data.size()) {
        LOG_DEBUG("Sent %d bytes", sent_bytes);
    } else {
        LOG_ERROR("Sent %d of %d bytes", bytes_sent, data.size());
    }
}

void GnuTlsConnection::receiveThreadFunc(GnuTlsConnection *self) {
    int bytes_received = 0;
    char recv_buffer[8191] = {0};
    while (self->m_receiveThreadActive) {
        bytes_received = gnutls_record_recv(self->session, recv_buffer, sizeof(recv_buffer));
        if (bytes_received < 0) {
            switch (bytes_received) {
            default:
                // ERROR
                LOG_ERROR("Error receiving data: %s\n", gnutls_strerror(bytes_received));
                self->onDisconnected();
                return;
                break;
            case GNUTLS_E_INTERRUPTED:
            case GNUTLS_E_AGAIN:
                continue;
            }
        } else {
            if (bytes_received == 0) {
                // Disconnected
                LOG_ERROR("Disconnected");
                self->onDisconnected();
            } else {
                LOG_DEBUG("Received %d bytes ", bytes_received);

                // Please note: we want a copy of the data so the receive buffer is
                // available for the next message
                std::vector<char> received_data(recv_buffer, recv_buffer + bytes_received);
                self->onData(received_data);
            }
        }
    }
}

int GnuTlsConnection::setConfig(const nlohmann::json &config) {
    try {
        if (config.contains("hostname") && config["hostname"].is_string()) {
            mHostName = config["hostname"];
        }
        if (config.contains("port") && config["port"].is_number_unsigned()) {
            mPort = config["port"];
        } else {
            mPort = 6697;
        }

        if (config.contains("ignoreInvalidCerficiate") && config["ignoreInvalidCerficiate"].is_boolean()) {
            ignoreInvalidCerficiate = config["ignoreInvalidCerficiate"];
        } else {
            ignoreInvalidCerficiate = false;
        }
        if (config.contains("ignoreInsecureProtocol") && config["ignoreInsecureProtocol"].is_boolean()) {
            ignoreInsecureProtocol = config["ignoreInsecureProtocol"];
        } else {
            ignoreInsecureProtocol = false;
        }

    } catch (nlohmann::json::exception &ex) {
        LOG_ERROR("JSON exception: %s", ex.what());
        return -1;
    } catch (std::exception &ex) {
        LOG_ERROR("Unknown exception: %s", ex.what());
        return -1;
    } catch (...) {
        LOG_ERROR("Unknown exception (not derived from std::exception)");
        return -1;
    }
    return 0;
}
} // namespace geblaat

#ifdef DYNAMIC_LIBRARY
extern "C" {
geblaat::GnuTlsConnection *newInstance(void) { return new geblaat::GnuTlsConnection(); }
void delInstance(geblaat::GnuTlsConnection *inst) { delete inst; }
pluginloadable_t plugin_info = {
    .name = "GnuTLS",
    .description = "TLS Connection support using GnuTLS",
    .abi = {.abi = pluginloadable_abi_cpp, .version = 0},
};
}
#endif
