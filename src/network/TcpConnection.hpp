/*
 * tcpconnection.hpp
 *
 *  Created on: 20 mrt. 2022
 *      Author: andre
 */


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


#include <atomic>
#include <thread>
#include "Connection.hpp"

namespace geblaat {

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

#if defined(_WIN32) || defined(_WIN64)
WSADATA d = {0};
#endif

    static void receiveThreadFunc(TcpConnection *self);
};

} // namespace network
