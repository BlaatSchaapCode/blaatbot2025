/*
 * tlsConnection.cpp
 *
 *  Created on: 7 mrt. 2025
 *      Author: andre
 */

#include <cstring>

#include "cTlsConnection.hpp"

namespace network {
void cTlsConnection::connect(std::string ip_address, uint16_t port, bool ignoreBadCertificate, bool ignoreLegacyServer) {
    int rc;
    rc = tls_init();
    m_tls_config = tls_config_new();

    if (ignoreBadCertificate) {
        tls_config_insecure_noverifycert(m_tls_config);
        tls_config_insecure_noverifyname(m_tls_config);
        tls_config_insecure_noverifytime(m_tls_config);
    } else {
        rc = tls_config_set_ca_file(m_tls_config, tls_default_ca_cert_file());
        tls_config_verify(m_tls_config);
    }

    if (ignoreLegacyServer) {
        uint32_t protocols = 0;
        rc = tls_config_parse_protocols(&protocols, "all");
        rc = tls_config_set_protocols(m_tls_config, protocols);

        rc = tls_config_set_ciphers(m_tls_config, "insecure");
    } else {
        uint32_t protocols = 0;
        rc = tls_config_parse_protocols(&protocols, "secure");
        rc = tls_config_set_protocols(m_tls_config, protocols);

        rc = tls_config_set_ciphers(m_tls_config, "secure");
    }

    rc = tls_config_set_dheparams(m_tls_config, "auto");
    rc = tls_config_set_ecdhecurves(m_tls_config, "default");

    m_tls_socket = tls_client();
    rc = tls_configure(m_tls_socket, m_tls_config);

    rc = tls_connect(m_tls_socket, ip_address.c_str(), std::to_string(port).c_str());
    if (rc) {
        LOG_ERROR("tls_connect: %s", tls_error(m_tls_socket));
        // TODO
        return;
    }

    rc = tls_handshake(m_tls_socket);
    if (rc < 0) {
        LOG_ERROR("tls_handshake: %s", tls_error(m_tls_socket));
        // TODO
        return;
    }

    printf("tls_conn_cipher = %s\n", tls_conn_cipher(m_tls_socket));
    printf("tls_conn_version = %s\n", tls_conn_version(m_tls_socket));

    m_receiveThreadActive = true;
    m_receiveThread = new std::thread(cTlsConnection::receiveThreadFunc, this);
    this->mProtocol->onConnected();
}

void cTlsConnection::receiveThreadFunc(cTlsConnection *self) {
    LOG_INFO("Starting Receive Thread");
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
            LOG_INFO("Received %d bytes ", bytes_received);

            // Please note: we want a copy of the data so the receive buffer is available for the next message
            std::vector<char> received_data(recv_buffer, recv_buffer + bytes_received);
            self->mProtocol->onData(received_data);
        }
    }
}

void cTlsConnection::send(std::vector<char> data) {
    size_t sent_bytes = ::tls_write(m_tls_socket, (const char *)data.data(), (int)data.size());
    if (sent_bytes == data.size()) {
        LOG_INFO("Sent %d bytes", sent_bytes);
    } else {
        LOG_ERROR("Sent %d of %d bytes", sent_bytes, data.size());
    }
}

cTlsConnection::~cTlsConnection() {}
} // namespace network
