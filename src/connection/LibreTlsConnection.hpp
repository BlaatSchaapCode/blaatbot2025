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

#ifndef NETWORK_TLSCONNECTION_HPP_
#define NETWORK_TLSCONNECTION_HPP_

#include <atomic>
#include <string>
#include <thread>

#include <tls.h>

#include "../connection/Connection.hpp"
#include "../utils/logger.hpp"

namespace geblaat {

class LibreTlsConnection : public Connection {

  public:
    using Connection::Connection;
    LibreTlsConnection();
    ~LibreTlsConnection();

    int connect() override;

    void send(std::vector<char> data) override;

    int setConfig(const nlohmann::json&) override;
    nlohmann::json getConfig(void) override;

  protected:
    bool ignoreInvalidCerficiate = false;
    bool ignoreInsecureProtocol = false;

  private:
    bool m_connected = false;
    nlohmann::json config;
    std::atomic<bool> m_receiveThreadActive = false;
    std::thread *m_receiveThread = nullptr;

    struct tls_config *m_tls_config = nullptr;
    struct tls *m_tls_socket = nullptr;

    static void receiveThreadFunc(LibreTlsConnection *self);
};

} // namespace geblaat

#endif /* NETWORK_TLSCONNECTION_HPP_ */
