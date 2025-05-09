/*
 * PluginLoader.hpp
 *
 *  Created on: 26 apr. 2025
 *      Author: andre
 */

#ifndef SRC_PLUGINLOADER_HPP_
#define SRC_PLUGINLOADER_HPP_

#include "clients/Client.hpp"
#include "network/Connection.hpp"
#include "protocol/Protocol.hpp"
#include "PluginLoadable.hpp"

#include <functional>
#include <map>



namespace geblaat {


class PluginLoader {
  public:
    Connection *newConnection(std::string type);
    Client *newClient(std::string type);
    Protocol *newProtocol(std::string type);

  private:
    struct plugin {
        std::string name;
        void *handle;
        int refcount;
    };
    struct networkPlugin : public plugin {
        std::function<::geblaat::Connection *(void)> newConnection;
        std::function<void(::geblaat::Connection *)> delConnection;
    };
    std::map<std::string, networkPlugin> networkPlugins;
    networkPlugin loadNetworkPlugin(std::string type);
};

}; // namespace geblaat
#endif /* SRC_PLUGINLOADER_HPP_ */
