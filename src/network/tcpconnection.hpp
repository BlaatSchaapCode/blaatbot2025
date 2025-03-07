/*
 * tcpconnection.hpp
 *
 *  Created on: 20 mrt. 2022
 *      Author: andre
 */

#include "network.hpp"

#include "connection.hpp"
#include <atomic>
#include <thread>

namespace network {

class TcpConnection : public iConnection {
  public:
    ~TcpConnection();

    void connect(std::string ip_address, uint16_t port);

    void send(std::vector<char> data) override;
    void send(std::string data) override;
    void process(void) override;

  private:
    struct sockaddr_in6 m_sin6 = {0};
    socket_t m_socket = 0;
    bool m_connected = false;

    std::atomic<bool> m_receiveThreadActive = false;
    std::thread *m_receiveThread = nullptr;

    static void receiveThreadFunc(TcpConnection *self);
};

} // namespace network
