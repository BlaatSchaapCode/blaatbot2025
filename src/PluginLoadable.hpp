/*
 * PluginLoadable.hpp
 *
 *  Created on: 9 mei 2025
 *      Author: andre
 */

#ifndef SRC_PLUGINLOADABLE_HPP_
#define SRC_PLUGINLOADABLE_HPP_

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

#endif /* SRC_PLUGINLOADABLE_HPP_ */
