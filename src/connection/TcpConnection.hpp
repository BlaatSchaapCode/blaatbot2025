/*
 * tcpconnection.hpp
 *
 *  Created on: 20 mrt. 2022
 *      Author: andre
 */

#if defined(__APPLE__) || defined(__DragonFly__) || defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__)
#define HAVE_SIN_LEN 1
#else
#define HAVE_SIN_LEN 0
#endif

#if defined(_WIN32) || defined(_WIN64)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <wspiapi.h>
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

} // namespace geblaat
