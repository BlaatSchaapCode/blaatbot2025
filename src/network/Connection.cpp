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
namespace geblaat {

Connection::~Connection() {}

void Connection::send(std::string s) {
    std::vector<char> v(s.begin(), s.end());
    send(v);
}

void Connection::setProtocol(::geblaat::Protocol *protocol) { mProtocol = protocol; }

// int Connection::setHostName(std::string hostName) {
//     mHostName = hostName;
//     return 0;
// }
// int Connection::setPort(uint16_t port) {
//     mPort = port;
//     return 0;
// }

} // namespace geblaat
