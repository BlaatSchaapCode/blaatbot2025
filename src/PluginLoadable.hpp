#pragma once

#include "PluginLoadable.h"

// Third Party libraries
#include <nlohmann/json.hpp>

namespace geblaat {
class PluginLoader;
class PluginLoadable {
  public:
    virtual ~PluginLoadable() {};
    virtual int setConfig(nlohmann::json) = 0;
    virtual void setPluginLoader(PluginLoader *pl) { pluginLoader = pl; }

  protected:
    PluginLoader *pluginLoader = nullptr;
};

}; // namespace geblaat
