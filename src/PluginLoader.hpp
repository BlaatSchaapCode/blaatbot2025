/*
 * PluginLoader.hpp
 *
 *  Created on: 26 apr. 2025
 *      Author: andre
 */

#ifndef SRC_PLUGINLOADER_HPP_
#define SRC_PLUGINLOADER_HPP_

// #include "PluginLoadable.hpp"
// #include "botmodule/BotModule.hpp"
// #include "clients/Client.hpp"
// #include "protocol/Protocol.hpp"

#include "connection/Connection.hpp"
#include <functional>
#include <map>

namespace geblaat {

class PluginLoader {
  public:
    // Connection *newConnection(std::string name);
    // Client *newClient(std::string name);
    // Protocol *newProtocol(std::string name);
    // BotModule *newBotModule(std::string name);

    PluginLoadable *newInstance(std::string name, std::string type);

#ifdef __i386__
    [[gnu::cdecl]] typedef char *(*geblaat_get_info_f)(void);
#else
    typedef char *(*geblaat_get_info_f)(void);
#endif

    struct plugin {
        std::string name;
        std::string type;
        void *handle;
        int refcount;
        std::function<PluginLoadable *(void)> newInstance;
        std::function<void(PluginLoadable *)> delInstance;
    };

    void *dlsym(void *handle, const char *symbol);
    void *dlopen(std::string type, std::string name);
    int dlclose(void *handle);
    std::string dlerror(void);

  private:
    std::map<std::string, plugin> plugins;
    plugin loadPlugin(std::string name, std::string type);
};

}; // namespace geblaat
#endif /* SRC_PLUGINLOADER_HPP_ */
