/*
 * cProtocol.cpp
 *
 *  Created on: 8 mrt. 2025
 *      Author: andre
 */

#include "cProtocol.hpp"
namespace protocol {
cProtocol::~cProtocol() {}
void cProtocol::setConnection(std::shared_ptr<::network::cConnection> connection) { mConnection = connection; }

} // namespace protocol
