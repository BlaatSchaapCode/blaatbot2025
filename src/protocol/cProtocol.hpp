/*
 * cProtocol.hpp
 *
 *  Created on: 8 mrt. 2025
 *      Author: andre
 */

#pragma once
#include "cconnection.hpp"
#include <memory>
#include <vector>

namespace network {
class cConnection;
}

namespace protocol {
class cProtocol {

  public:
    virtual ~cProtocol() = 0;
    virtual void onData(std::vector<char> data) = 0;
    virtual void onConnected() = 0;
    virtual void onDisconnected() = 0;
    void setConnection(::network::cConnection *connection) { mConnection = connection; }

  protected:
    ::network::cConnection *mConnection = nullptr;
};

} // namespace protocol
