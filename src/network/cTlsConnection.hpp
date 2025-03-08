/*
 * tlcConnection.hpp
 *
 *  Created on: 7 mrt. 2025
 *      Author: andre
 */

#ifndef NETWORK_CTLSCONNECTION_HPP_
#define NETWORK_CTLSCONNECTION_HPP_

#include "network.hpp"

#include <atomic>
#include <string>
#include <thread>

#include <tls.h>

#include "../utils/logger.hpp"
#include "cconnection.hpp"

namespace network {

class cTlsConnection : public cConnection {

  public:
    using cConnection::cConnection;
    ~cTlsConnection();

    void connect(std::string ip_address, uint16_t port = 6697, bool ignoreServerName = false, bool allowDeprecatedServer = false);

    void send(std::vector<char> data) override;
    void send(std::string data) override;

  private:
    bool m_connected = false;

    std::atomic<bool> m_receiveThreadActive = false;
    std::thread *m_receiveThread = nullptr;

    struct tls_config *m_tls_config = nullptr;
    struct tls *m_tls_socket = nullptr;

    static void receiveThreadFunc(cTlsConnection *self);
};

} // namespace network

#endif /* NETWORK_CTLSCONNECTION_HPP_ */
