/*
 * connection.cpp
 *
 *  Created on: 7 mrt. 2025
 *      Author: andre
 */

#include "cconnection.hpp"

#include <iostream>
#include <string>
#include <vector>

#include "cProtocol.hpp"
namespace network {

cConnection::cConnection(std::shared_ptr<::protocol::cProtocol> protocol) {
    mProtocol = protocol;
    //		auto p = shared_from_this();
    //		mProtocol->setConnection(p);
}

cConnection::~cConnection() {}

} // namespace network
