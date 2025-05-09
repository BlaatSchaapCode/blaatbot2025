#pragma once

// C++ Library Includes
#include <map>
#include <memory>
#include <string>

// Third Party libraries
#include <nlohmann/json.hpp>

// Project includes
#include "../protocol/IRC.hpp"
#include "../protocol/Protocol.hpp"

#include "PluginLoader.hpp"

namespace geblaat {

class Client : public PluginLoadable {
  public:
    Client();
    virtual ~Client();
    virtual int setConfig(nlohmann::json) override;

    virtual void onMessage(std::map<std::string, std::string> message);

  protected:
    Protocol *mProtocol = nullptr;
    //    Connection *mConnection  = nullptr;
};

} // namespace geblaat
