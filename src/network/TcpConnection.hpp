/*
 * tcpconnection.hpp
 *
 *  Created on: 20 mrt. 2022
 *      Author: andre
 */

#include "network.hpp"

#include <atomic>
#include <thread>
#include "Connection.hpp"

namespace network {

class cTcpConnection : public cConnection {
  public:
    ~cTcpConnection();

    void connect(std::string ip_address, uint16_t port = 6667);

    void send(std::vector<char> data) override;

  private:
    struct sockaddr_in6 m_sin6 = {0};
    socket_t m_socket = 0;
    bool m_connected = false;

    std::atomic<bool> m_receiveThreadActive = false;
    std::thread *m_receiveThread = nullptr;

    static void receiveThreadFunc(cTcpConnection *self);
};

} // namespace network
