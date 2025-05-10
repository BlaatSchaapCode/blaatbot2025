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

#include "PluginLoadable.hpp"

namespace geblaat {
class Client;
class Connection;

class Protocol : public PluginLoadable {

  public:
    virtual ~Protocol() = 0;
    virtual void onData(std::vector<char> data) = 0;
    virtual void onConnected() = 0;
    virtual void onDisconnected() = 0;
    void setConnection(Connection *connection) { mConnection = connection; }
    void setClient(Client *client) { mClient = client; }

    virtual int setConfig(nlohmann::json) = 0;
    virtual void sendMessage(std::map<std::string, std::string> message) = 0;

  protected:
    Connection *mConnection = nullptr;
    Client *mClient = nullptr;
};

} // namespace geblaat
