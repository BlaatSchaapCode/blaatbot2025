/*
 * cProtocol.hpp
 *
 *  Created on: 8 mrt. 2025
 *      Author: andre
 */

#pragma once
#include <memory>
#include <vector>
#include "../network/Connection.hpp"

namespace network {
class Connection;
}

namespace protocol {
class Protocol {

  public:
    virtual ~Protocol() = 0;
    virtual void onData(std::vector<char> data) = 0;
    virtual void onConnected() = 0;
    virtual void onDisconnected() = 0;
    void setConnection(::network::Connection *connection) { mConnection = connection; }

  protected:
    ::network::Connection *mConnection = nullptr;
};

} // namespace protocol
