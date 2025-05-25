/*
 * tlsConnection.cpp
 *
 *  Created on: 7 mrt. 2025
 *      Author: andre
 */

#include "../connection/TlsConnection.hpp"

#include <cstring>

#include "threadName.hpp"

namespace geblaat {
// void cTlsConnection::connect(std::string ip_address, uint16_t port, bool
// ignoreBadCertificate, bool ignoreLegacyServer) {
int TlsConnection::connect(void) {
    int rc;
    rc = tls_init();
    m_tls_config = tls_config_new();

    if (ignoreInvalidCerficiate) {
        tls_config_insecure_noverifycert(m_tls_config);
        tls_config_insecure_noverifyname(m_tls_config);
        tls_config_insecure_noverifytime(m_tls_config);
    } else {
        rc = tls_config_set_ca_file(m_tls_config, tls_default_ca_cert_file());
        if (rc < 0) {
            LOG_ERROR("tls_config_set_ca_file: %s", tls_error(m_tls_socket));
            return rc;
        }
        tls_config_verify(m_tls_config);
    }

    if (ignoreInsecureProtocol) {
        uint32_t protocols = 0;

        // libtls dropped support for older protocols,
        // I can't force it to use them if I try.
        // "legacy" has no effect and selects 1.2 and 1.3

        rc = tls_config_parse_protocols(&protocols, "legacy");
        if (rc < 0) {
            LOG_ERROR("tls_config_parse_protocols: %s", tls_error(m_tls_socket));
            return rc;
        }

        rc = tls_config_set_protocols(m_tls_config, protocols);
        if (rc < 0) {
            LOG_ERROR("tls_config_set_protocols: %s", tls_error(m_tls_socket));
            return rc;
        }

        rc = tls_config_set_ciphers(m_tls_config, "insecure");
        if (rc < 0) {
            LOG_ERROR("tls_config_set_ciphers: %s", tls_error(m_tls_socket));
            return rc;
        }
    } else {
        uint32_t protocols = 0;
        rc = tls_config_parse_protocols(&protocols, "secure");
        if (rc < 0) {
            LOG_ERROR("tls_config_parse_protocols: %s", tls_error(m_tls_socket));
            return rc;
        }
        rc = tls_config_set_protocols(m_tls_config, protocols);
        if (rc < 0) {
            LOG_ERROR("tls_config_set_protocols: %s", tls_error(m_tls_socket));
            return rc;
        }
        rc = tls_config_set_ciphers(m_tls_config, "secure");
        if (rc < 0) {
            LOG_ERROR("tls_config_set_ciphers: %s", tls_error(m_tls_socket));
            return rc;
        }
    }

    rc = tls_config_set_dheparams(m_tls_config, "auto");
    if (rc < 0) {
        LOG_ERROR("tls_config_set_dheparams: %s", tls_error(m_tls_socket));
        return rc;
    }

    rc = tls_config_set_ecdhecurves(m_tls_config, "default");
    if (rc < 0) {
        LOG_ERROR("tls_config_set_ecdhecurves: %s", tls_error(m_tls_socket));
        return rc;
    }

    m_tls_socket = tls_client();
    rc = tls_configure(m_tls_socket, m_tls_config);
    if (rc < 0) {
        LOG_ERROR("tls_configure: %s", tls_error(m_tls_socket));
        return rc;
    }

    rc = tls_connect(m_tls_socket, mHostName.c_str(), std::to_string(mPort).c_str());
    if (rc < 0) {
        LOG_ERROR("tls_connect: %s", tls_error(m_tls_socket));
        return rc;
    }

    rc = tls_handshake(m_tls_socket);
    if (rc < 0) {
        LOG_ERROR("tls_handshake: %s", tls_error(m_tls_socket));
        return rc;
    }

    printf("tls_conn_cipher = %s\n", tls_conn_cipher(m_tls_socket));
    printf("tls_conn_version = %s\n", tls_conn_version(m_tls_socket));

    m_receiveThreadActive = true;
    m_receiveThread = new std::thread(TlsConnection::receiveThreadFunc, this);
    this->mProtocol->onConnected();
    return 0;
}

void TlsConnection::receiveThreadFunc(TlsConnection *self) {
    LOG_INFO("Starting Receive Thread");
    setThreadName("TlsRecv");
    char recv_buffer[8191] = {0};
    // TODO: exit conditions
    // Depends on:
    //	* setting  timeouts on sockets
    //  * termination signalling ( atomic )

    int bytes_received = 0;
    while (self->m_receiveThreadActive) {
        bytes_received = tls_read(self->m_tls_socket, recv_buffer, sizeof(recv_buffer));
        if (bytes_received < 0) {
            if (bytes_received == -1) {
                LOG_ERROR("tls_read: %s", tls_error(self->m_tls_socket));
            }
            continue;
        } else if (bytes_received == 0) {
            LOG_ERROR("Remote disconnected");
            self->mProtocol->onDisconnected();
            break;
        } else {
            // Todo: pass data to parser
            LOG_DEBUG("Received %d bytes ", bytes_received);

            // Please note: we want a copy of the data so the receive buffer is
            // available for the next message
            std::vector<char> received_data(recv_buffer, recv_buffer + bytes_received);
            self->mProtocol->onData(received_data);
        }
    }
}

void TlsConnection::send(std::vector<char> data) {
    size_t sent_bytes = ::tls_write(m_tls_socket, (const char *)data.data(), (int)data.size());
    if (sent_bytes == data.size()) {
        LOG_DEBUG("Sent %d bytes", sent_bytes);
    } else {
        LOG_ERROR("Sent %d of %d bytes", sent_bytes, data.size());
    }
}

int TlsConnection::setConfig(nlohmann::json config) {
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

TlsConnection::TlsConnection() { mPort = 6697; }
TlsConnection::~TlsConnection() {
    LOG_INFO("Stopping receive thread ");
    m_receiveThreadActive = false;
    LOG_INFO("Closing socket");
    tls_close(m_tls_socket);
    if (m_receiveThread->joinable())
        m_receiveThread->join();
    LOG_INFO("Deleting Receive Thread", 0);
    delete m_receiveThread;
}

} // namespace geblaat

#ifdef DYNAMIC_LIBRARY
extern "C" {
geblaat::TlsConnection *newInstance(void) { return new geblaat::TlsConnection(); }
void delInstance(geblaat::TlsConnection *inst) { delete inst; }
pluginloadable_t plugin_info = {
    .name = "LibreTLS",
    .description = "TLS Connection support using LibreTLS",
    .abi = {.abi = pluginloadable_abi_cpp, .version = 0},
};
}
#endif
