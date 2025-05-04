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

class TcpConnection : public Connection {
  public:
    ~TcpConnection();
    TcpConnection();

    int connect(void) override;
    void send(std::vector<char> data) override;

    int setConfig(nlohmann::json) override;

  private:
    struct sockaddr_in6 in6 = {};
    struct sockaddr_in in = {};
    socket_t m_socket = 0;
    bool m_connected = false;

    std::atomic<bool> m_receiveThreadActive = false;
    std::thread *m_receiveThread = nullptr;

    static void receiveThreadFunc(TcpConnection *self);
};

} // namespace network
