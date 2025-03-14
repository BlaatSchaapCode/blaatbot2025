/*
 * tcpconnection.cpp
 *
 *  Created on: 20 mrt. 2022
 *      Author: andre
 */

#include "network.hpp"

#include <cstring>
#include <errno.h>
#include <iostream>
#include <random>
#include <vector>

#include "../utils/logger.hpp"
#include "cTcpConnection.hpp"

namespace network {

cTcpConnection::~cTcpConnection() {
    m_receiveThreadActive = false;
    LOG_INFO("Stopping receive thread ", 0);
    if (m_receiveThread->joinable()) {
        LOG_INFO("receive thread joinable", 1);
        m_receiveThread->join();
    } else {
        LOG_INFO("receive thread not joinable", 0);
    }
    LOG_INFO("Deleting Receive Thread", 0);
    delete m_receiveThread;
    LOG_INFO("Closing socket", 0);
    closesocket(m_socket);
}

void cTcpConnection::send(std::vector<char> data) {
    size_t sent_bytes = ::send(m_socket, (const char *)data.data(), (int)data.size(), 0);
    if (sent_bytes == data.size()) {
        LOG_INFO("Sent %d bytes", sent_bytes);
    } else {
        LOG_ERROR("Sent %d of %d bytes", sent_bytes, data.size());
    }
}

void cTcpConnection::receiveThreadFunc(cTcpConnection *self) {
    LOG_INFO("Starting Receive Thread");
    char recv_buffer[8191] = {0};
    // TODO: exit conditions
    // Depends on:
    //	* setting  timeouts on sockets
    //  * termination signalling ( atomic )

    int bytes_received = 0;
    while (self->m_receiveThreadActive) {
        bytes_received = recv(self->m_socket, recv_buffer, sizeof(recv_buffer), 0);
        if (bytes_received < 0) {
            // If there is any other error then timeout
            if (EWOULDBLOCK != errno) {
#ifdef _MSC_FULL_VER
                char buffer[120];
                strerror_s(buffer, sizeof(buffer), errno);
                LOG_ERROR("Error reading from socket %d: %s ", errno, buffer);
#else
                LOG_ERROR("Error reading from socket %d: %s ", errno, strerror(errno));
#endif
                break;
            }
            // There is no data
            continue;
        } else if (bytes_received == 0) {
            LOG_ERROR("Remote disconnected");
            // on_disconnect(self);  // TODO
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

void cTcpConnection::connect(std::string host, uint16_t port) {
    LOG_INFO("Requested to connect to %s:%d", host.c_str(), port);

    m_socket = 0;

    struct addrinfo hints, *res, *result;
    int errcode;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags |= AI_CANONNAME;

    errcode = getaddrinfo(host.c_str(), NULL, &hints, &result);
    if (errcode != 0) {
        LOG_ERROR("getaddrinfo");
        return;
    }

    res = result;

    struct sockaddr_in6 in6 = {};
    struct sockaddr_in in = {};

    while (res && !m_connected) {
        switch (res->ai_family) {
        case AF_INET: {
            in = *(struct sockaddr_in *)res->ai_addr;
            in.sin_port = htons(port);

            char temp[INET_ADDRSTRLEN];
            LOG_INFO("%s resolved to %s", host.c_str(), inet_ntop(AF_INET, &in.sin_addr, temp, INET_ADDRSTRLEN));

            m_socket = ::socket(AF_INET, SOCK_STREAM, 0);
            if (m_socket < 0) {
                LOG_ERROR("Error creating socket");
                return;
            }
            if (::connect(m_socket, (const sockaddr *)&in, sizeof(sockaddr_in)) < 0) {
                LOG_INFO("Failed to connect");
                close(m_socket);
                m_socket = 0;
            } else {
                LOG_INFO("Connected");
                m_connected = true;
            }
        } break;
        case AF_INET6: {
            in6 = *(struct sockaddr_in6 *)res->ai_addr;
            in6.sin6_port = htons(port);
            char temp[INET6_ADDRSTRLEN];
            LOG_INFO("%s resolved to %s", host.c_str(), inet_ntop(AF_INET6, &in6.sin6_addr, temp, INET6_ADDRSTRLEN));

            m_socket = ::socket(AF_INET6, SOCK_STREAM, 0);
            if (m_socket < 0) {
                LOG_ERROR("Error creating socket");
                return;
            }
            if (::connect(m_socket, (const sockaddr *)&in6, sizeof(sockaddr_in6)) < 0) {
                LOG_INFO("Failed to connect");
                close(m_socket);
                m_socket = 0;
            } else {
                LOG_INFO("Connected");
                m_connected = true;
            }
        } break;
        }

        res = res->ai_next;
    }

    freeaddrinfo(result);

    if (!m_connected) {
        LOG_ERROR("Not Connected");
        return;
    }

    LOG_INFO("Connected");

    //--------------------------------------------
    // Configure socket options for the new socket
    //--------------------------------------------
    const int no = 0;
    (void)no;
    const int yes = 1;
    (void)yes;
    // Set timeouts for send and receive in blocking mode
    const struct timeval tv = {.tv_sec = 0, .tv_usec = 100000};
    setsockopt(m_socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv));
    setsockopt(m_socket, SOL_SOCKET, SO_SNDTIMEO, (const char *)&tv, sizeof(tv));

    setsockopt(m_socket, SOL_SOCKET, SO_KEEPALIVE, (const char *)&yes, sizeof(yes));

    m_receiveThreadActive = true;
    m_receiveThread = new std::thread(cTcpConnection::receiveThreadFunc, this);
    this->mProtocol->onConnected();
}

} // namespace network
