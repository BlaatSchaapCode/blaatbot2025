/*
 * cProtocol.hpp
 *
 *  Created on: 8 mrt. 2025
 *      Author: andre
 */

#pragma once

// C++ Includes
#include <memory>
#include <vector>

// Third Party libraries
#include <nlohmann/json.hpp>

// Project Includes
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

    virtual int setConfig(nlohmann::json) = 0;

  protected:
    ::network::Connection *mConnection = nullptr;
};

} // namespace protocol
