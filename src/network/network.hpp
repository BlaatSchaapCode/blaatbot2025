#pragma once

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

namespace network {
void init(void);
void deinit(void);
}; // namespace network
