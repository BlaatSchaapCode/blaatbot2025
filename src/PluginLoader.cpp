/*
 * PluginLoader.cpp
 *
 *  Created on: 26 apr. 2025
 *      Author: andre
 */

#include "PluginLoader.hpp"

#include <dlfcn.h>

#include <exception>

#include "utils/logger.hpp"

::network::Connection *PluginLoader::newConnection(std::string name) {
    if (this->networkPlugins.contains(name)) {
        this->networkPlugins[name].refcount++;
        return this->networkPlugins[name].newConnection();
    } else {
        try {
            this->networkPlugins[name] = loadNetworkPlugin(name);
            return this->networkPlugins[name].newConnection();
        } catch (std::exception &ex) {
            LOG_ERROR("Error: %s", ex.what());
        }
    }

    return nullptr;
}

PluginLoader::networkPlugin PluginLoader::loadNetworkPlugin(std::string name) {
    networkPlugin result;
    result.refcount = 1;
    result.name = name;
    std::string library = "libgeblaat_network_" + name + ".so";

    typedef ::network::Connection *(*newInstance_f)();
    newInstance_f newInstance;
    typedef void (*delInstance_f)(::network::Connection *);
    delInstance_f delInstance;

    result.handle = dlopen(library.c_str(), RTLD_NOW);
    if (result.handle == nullptr) {
        throw std::runtime_error(dlerror());
    }

    newInstance = (newInstance_f)dlsym(result.handle, "newInstance");
    if (!newInstance) {
        throw std::runtime_error(dlerror());
    }

    delInstance = (delInstance_f)dlsym(result.handle, "delInstance");
    if (!delInstance) {
        throw std::runtime_error(dlerror());
    }

    int * test =  (int *)dlsym(result.handle, "test");


    result.newConnection = newInstance;
    result.delConnection = delInstance;

    return result;
}
