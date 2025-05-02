/*
 * PluginLoader.hpp
 *
 *  Created on: 26 apr. 2025
 *      Author: andre
 */

#ifndef SRC_PLUGINLOADER_HPP_
#define SRC_PLUGINLOADER_HPP_

#include "network/Connection.hpp"

#include <functional>
#include <map>




class PluginLoader {
  public:
    ::network::Connection *newConnection(std::string type);

  private:
    struct plugin {
        std::string name;
        void *handle;
        int refcount;
    };
    struct networkPlugin : public plugin {
        std::function<::network::Connection *(void)> newConnection;
        std::function<void(::network::Connection *)> delConnection;
    };
    std::map<std::string, networkPlugin> networkPlugins;
    networkPlugin loadNetworkPlugin(std::string type);
};

#endif /* SRC_PLUGINLOADER_HPP_ */
