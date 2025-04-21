/*
 * connection.cpp
 *
 *  Created on: 7 mrt. 2025
 *      Author: andre
 */

#include <iostream>
#include <string>
#include <vector>



#include "Connection.hpp"
namespace network {

cConnection::~cConnection() {}

void cConnection::send(std::string s) {
    std::vector<char> v(s.begin(), s.end());
    send(v);
}

void cConnection::setProtocol(::protocol::Protocol *protocol) { mProtocol = protocol; }

int cConnection::setHostName(std::string hostName) {
    mHostName = hostName;
    return 0;
}
int cConnection::setPort(uint16_t port) {
    mPort = port;
    return 0;
}

} // namespace network
