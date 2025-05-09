#pragma once

// C++ Library Includes
#include <map>
#include <memory>
#include <string>

// Third Party libraries
#include <nlohmann/json.hpp>

// Project includes
#include "../protocol/Protocol.hpp"

#include "PluginLoader.hpp"

namespace geblaat {

class Client : public PluginLoadable {
  public:
    virtual ~Client() = 0;
    virtual int setConfig(nlohmann::json) = 0;

    virtual void onMessage(std::map<std::string, std::string> message) = 0;

};

} // namespace geblaat
