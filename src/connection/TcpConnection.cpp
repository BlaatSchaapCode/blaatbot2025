/*

 Author:	André van Schoubroeck <andre@blaatschaap.be>
 License:	MIT

 SPDX-License-Identifier: MIT

 Copyright (c) 2025 André van Schoubroeck <andre@blaatschaap.be>

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 */

#include "../connection/TcpConnection.hpp"

#include <iostream>
#include <random>
#include <vector>

#include <cerrno>
#include <cstring>

#include <sys/time.h>

#include "logger.hpp"
#include "threadName.hpp"

namespace geblaat {
void TcpConnection::send(std::vector<char> data) {
    size_t sent_bytes = ::send(m_socket, (const char *)data.data(), (int)data.size(), 0);
    if (sent_bytes == data.size()) {
        LOG_DEBUG("Sent %d bytes", sent_bytes);
    } else {
        LOG_ERROR("Sent %d of %d bytes", sent_bytes, data.size());
    }
}

void TcpConnection::onData(std::vector<char> received_data) { mProtocol->onData(received_data); }
void TcpConnection::onConnected() {
    m_receiveThreadActive = true;
    m_receiveThread = new std::thread(TcpConnection::receiveThreadFunc, this);
    mProtocol->onConnected();
}
void TcpConnection::onDisconnected() { mProtocol->onDisconnected(); }

void TcpConnection::receiveThreadFunc(TcpConnection *self) {
    LOG_INFO("Starting Receive Thread");
    setThreadName("TcpRecv");
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
            if (EWOULDBLOCK != errno && errno != EINTR) {
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
            self->onDisconnected();
            break;
        } else {
            LOG_DEBUG("Received %d bytes ", bytes_received);

            // Please note: we want a copy of the data so the receive buffer is
            // available for the next message
            std::vector<char> received_data(recv_buffer, recv_buffer + bytes_received);
            self->onData(received_data);
        }
    }
}

int TcpConnection::connect(void) {
    LOG_INFO("Requested to connect to %s:%d", mHostName.c_str(), mPort);

    m_socket = 0;

    struct addrinfo hints, *res, *result;
    int errcode;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags |= AI_CANONNAME;

    errcode = getaddrinfo(mHostName.c_str(), NULL, &hints, &result);
    if (errcode != 0) {
        LOG_ERROR("getaddrinfo");
        return errcode;
    }

    res = result;

    struct sockaddr_in6 in6 = {};
    struct sockaddr_in in = {};

    while (res && !m_connected) {
        switch (res->ai_family) {
        case AF_INET: {
            in = *(struct sockaddr_in *)res->ai_addr;
            in.sin_port = htons(mPort);

            char temp[INET_ADDRSTRLEN];
            LOG_INFO("%s resolved to %s", mHostName.c_str(), inet_ntop(AF_INET, &in.sin_addr, temp, INET_ADDRSTRLEN));

            m_socket = ::socket(AF_INET, SOCK_STREAM, 0);
            if (m_socket < 0) {
                LOG_ERROR("Error creating socket");
                return m_socket;
            }
            if (::connect(m_socket, (const sockaddr *)&in, sizeof(sockaddr_in)) < 0) {
                LOG_INFO("Failed to connect");
                closesocket(m_socket);
                m_socket = 0;
            } else {
                LOG_INFO("Connected");
                m_connected = true;
            }
        } break;
        case AF_INET6: {
            in6 = *(struct sockaddr_in6 *)res->ai_addr;
            in6.sin6_port = htons(mPort);
            char temp[INET6_ADDRSTRLEN];
            LOG_INFO("%s resolved to %s", mHostName.c_str(), inet_ntop(AF_INET6, &in6.sin6_addr, temp, INET6_ADDRSTRLEN));

            m_socket = ::socket(AF_INET6, SOCK_STREAM, 0);
            if (m_socket < 0) {
                LOG_ERROR("Error creating socket");
                return m_socket;
            }
            if (::connect(m_socket, (const sockaddr *)&in6, sizeof(sockaddr_in6)) < 0) {
                LOG_INFO("Failed to connect");
                closesocket(m_socket);
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
        return -ENOTCONN;
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

    onConnected();
    return 0;
}

int TcpConnection::setConfig(const nlohmann::json &cfg) {
    try {
        config = cfg;
        if (config.contains("hostname") && config["hostname"].is_string()) {
            mHostName = config["hostname"];
        }
        if (config.contains("port") && config["port"].is_number_unsigned()) {
            mPort = config["port"];
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

TcpConnection::TcpConnection() {
    mPort = 6667;

#if defined(_WIN32) || defined(_WIN64)
    if (WSAStartup(0x0202, &d)) {
        LOG_ERROR("Error initialising WinSock");
    }
#endif
}

TcpConnection::~TcpConnection() {
    LOG_INFO("Stopping receive thread ", 0);
    m_receiveThreadActive = false;
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

#if defined(_WIN32) || defined(__WIN32__)
    WSACleanup();
#endif
}

} // namespace geblaat

#if (defined DYNAMIC_LIBRARY)
extern "C" {
[[gnu::weak]] geblaat::TcpConnection *newInstance(void) { return new geblaat::TcpConnection(); }
[[gnu::weak]] void delInstance(geblaat::TcpConnection *inst) { delete inst; }
[[gnu::weak]] pluginloadable_t plugin_info = {
    .name = "TCP Connection",
    .description = "TCP connection support",
    .abi = {.abi = pluginloadable_abi_cpp, .version = 0},
};
}
#endif
