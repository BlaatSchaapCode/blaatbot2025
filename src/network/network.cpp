#include "network.hpp"

#include <cstdint>
#include <cstring>

#include <algorithm>
#include <atomic>
#include <iostream>
#include <map>
#include <random>
#include <string>
#include <thread>
#include <vector>

#include "connection.hpp"
#include "tcpconnection.hpp"

#include "../utils/logger.hpp"

namespace network {

std::atomic<bool> active = false;

#if defined(_WIN32) || defined(__WIN32__)
WSADATA d = {0};
#endif

void init(void) {
#if defined(_WIN32) || defined(__WIN32__)
    if (WSAStartup(0x0202, &d)) {
        LOG_ERROR("Error initialising WinSock");
    }
#endif

    active = true;
}

void deinit(void) {
    active = false;

#if defined(_WIN32) || defined(__WIN32__)
    WSACleanup();
#endif
}

}; // namespace network
