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
    PluginLoader();
    PluginLoadable *newInstance(std::string name, std::string type);

#ifdef __i386__
    [[gnu::cdecl]] typedef char *(*geblaat_get_info_f)(void);
#else
    typedef char *(*geblaat_get_info_f)(void);
#endif

    struct Plugin {
        std::string name;
        std::string type;
        void *handle;
        int refcount;
        std::function<PluginLoadable *(void)> newInstance;
        std::function<void(PluginLoadable *)> delInstance;
    };

    using Loader = std::function<void(Plugin &)>;
    void registerPluginLoader(Loader);

    // Exposing library functions, OS abstracted.
    // Can be used by external loaders
    void *dlsym(void *handle, const char *symbol);
    void *dlopen(std::string type, std::string name);
    int dlclose(void *handle);
    std::string dlerror(void);

  private:
    std::map<std::string, Plugin> plugins;
    Plugin loadPlugin(std::string name, std::string type);
    void loadCppPlugin(Plugin &);
    std::vector<Loader> loaders;
};

}; // namespace geblaat
#endif /* SRC_PLUGINLOADER_HPP_ */
