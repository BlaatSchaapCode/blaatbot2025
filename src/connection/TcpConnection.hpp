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

#if defined(__APPLE__) || defined(__DragonFly__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#define HAVE_SIN_LEN 1
#else
#define HAVE_SIN_LEN 0
#endif

#if defined(_WIN32) || defined(_WIN64)
#include <winsock2.h>
#include <ws2tcpip.h>

typedef SOCKET socket_t;
#define poll WSAPoll

#else
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

typedef int socket_t;
#define closesocket close

#endif

#include "../connection/Connection.hpp"
#include <atomic>
#include <thread>

namespace geblaat {

class TcpConnection : public Connection {
  public:
    ~TcpConnection();
    TcpConnection();

    int connect(void) override;
    void send(std::vector<char> data) override;

    int setConfig(const nlohmann::json &) override;
    nlohmann::json getConfig(void) override { return config; }

  protected:
    nlohmann::json config;

    socket_t m_socket = 0;
    bool m_connected = false;

    std::atomic<bool> m_receiveThreadActive = false;
    std::thread *m_receiveThread = nullptr;

    virtual void onData(std::vector<char> data);
    virtual void onConnected();
    virtual void onDisconnected();

  private:
#if defined(_WIN32) || defined(_WIN64)
    WSADATA d = {0};
#endif

    static void receiveThreadFunc(TcpConnection *self);
};

} // namespace geblaat
