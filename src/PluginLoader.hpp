/*
 * PluginLoader.hpp
 *
 *  Created on: 26 apr. 2025
 *      Author: andre
 */

#ifndef SRC_PLUGINLOADER_HPP_
#define SRC_PLUGINLOADER_HPP_

#include "clients/Client.hpp"
#include "protocol/Protocol.hpp"
#include "PluginLoadable.hpp"
#include "botmodule/BotModule.hpp"

#include <functional>
#include <map>
#include "connection/Connection.hpp"



namespace geblaat {


class PluginLoader {
  public:
    Connection *newConnection(std::string type);
    Client *newClient(std::string type);
    Protocol *newProtocol(std::string type);
    BotModule *newBotModule(std::string type);


#ifdef __i386__
    [[gnu::cdecl]] typedef char *(*geblaat_get_info_f)(void);
#else
    typedef char *(*geblaat_get_info_f)(void);
#endif

  private:
    struct plugin {
        std::string name;
        std::string type;
        void *handle;
        int refcount;
		std::function<PluginLoadable *(void)> newInstance;
		std::function<void(PluginLoadable *)> delInstance;
    };

    std::map<std::string, plugin> plugins;
    plugin loadPlugin(std::string name, std::string type);
};

}; // namespace geblaat
#endif /* SRC_PLUGINLOADER_HPP_ */
