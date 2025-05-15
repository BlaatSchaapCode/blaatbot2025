/*
 * PluginLoader.cpp
 *
 *  Created on: 26 apr. 2025
 *      Author: andre
 */

#include "PluginLoader.hpp"

#include <exception>

#include "utils/classname.hpp"
#include "utils/logger.hpp"

#include "botmodule/CAPI_BotModule.hpp"

namespace geblaat {

Client *PluginLoader::newClient(std::string name) {
    if (!this->plugins.contains("client_" + name)) {
        try {
            this->plugins["client_" + name] = loadPlugin(name, "client");
        } catch (std::exception &ex) {
            LOG_ERROR("Error: %s", ex.what());
        }
    }

    if (this->plugins.contains("client_" + name)) {
        auto instance = this->plugins["client_" + name].newInstance();
        LOG_INFO("Got an instance of %s", demangleClassName(typeid(instance).name()).c_str());
        Client *clientInstance = dynamic_cast<Client *>(instance);
        if (clientInstance) {
            LOG_INFO("Got an instance of %s", demangleClassName(typeid(clientInstance).name()).c_str());
            this->plugins["client_" + name].refcount++;
            clientInstance->setPluginLoader(this);
            return clientInstance;
        }
        LOG_ERROR("Error: Not a Client plugin");
    } else {
        LOG_ERROR("Error: Plugin not found");
    }

    return nullptr;
}

Protocol *PluginLoader::newProtocol(std::string name) {
    if (!this->plugins.contains("protocol_" + name)) {
        try {
            this->plugins["protocol_" + name] = loadPlugin(name, "protocol");
        } catch (std::exception &ex) {
            LOG_ERROR("Error: %s", ex.what());
        }
    }

    if (this->plugins.contains("protocol_" + name)) {
        auto instance = this->plugins["protocol_" + name].newInstance();
        LOG_INFO("Got an instance of %s", demangleClassName(typeid(instance).name()).c_str());
        Protocol *protocolInstance = dynamic_cast<Protocol *>(instance);
        if (protocolInstance) {
            LOG_INFO("Got an instance of %s", demangleClassName(typeid(protocolInstance).name()).c_str());
            this->plugins["protocol_" + name].refcount++;
            protocolInstance->setPluginLoader(this);
            return protocolInstance;
        }
        LOG_ERROR("Error: Not a Connection plugin");
    } else {
        LOG_ERROR("Error: Plugin not found");
    }

    return nullptr;
}

BotModule *PluginLoader::newBotModule(std::string name) {
    if (!this->plugins.contains("botmodule_" + name)) {
        try {
            this->plugins["botmodule_" + name] = loadPlugin(name, "botmodule");
        } catch (std::exception &ex) {
            LOG_ERROR("Error: %s", ex.what());
        }
    }

    if (this->plugins.contains("botmodule_" + name)) {
        BotModule *instance = dynamic_cast<BotModule *>(this->plugins["botmodule_" + name].newInstance());
        if (instance) {
            this->plugins["botmodule_" + name].refcount++;
            instance->setPluginLoader(this);
            return instance;
        }
        LOG_ERROR("Error: Not a BotModule plugin");
    } else {
        LOG_ERROR("Error: Plugin not found");
    }

    return nullptr;
}

Connection *PluginLoader::newConnection(std::string name) {
    if (!this->plugins.contains("connection_" + name)) {
        try {
            this->plugins["connection_" + name] = loadPlugin(name, "connection");
        } catch (std::exception &ex) {
            LOG_ERROR("Error: %s", ex.what());
        }
    }

    if (this->plugins.contains("connection_" + name)) {
        Connection *instance = dynamic_cast<Connection *>(this->plugins["connection_" + name].newInstance());
        if (instance) {
            this->plugins["connection_" + name].refcount++;
            instance->setPluginLoader(this);
            return instance;
        }
        LOG_ERROR("Error: Not a Connection plugin");
    } else {
        LOG_ERROR("Error: Plugin not found");
    }
    return nullptr;
}

#if defined(_WIN32) || defined(_WIN64)

#include <windows.h>
// #include <libloaderapi.h>

static std::string getWin32ErrorString() {
    // https://stackoverflow.com/questions/1387064/how-to-get-the-error-message-from-the-error-code-returned-by-getlasterror

    DWORD errorMessageID = GetLastError();
    LPSTR messageBuffer = nullptr;

    // Ask Win32 to give us the string version of that message ID.
    // The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long
    // the message string will be).
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
                                 errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    // Copy the error message into a std::string.
    std::string message(messageBuffer, size);

    // Free the Win32's string's buffer.
    LocalFree(messageBuffer);
    return message;
}

PluginLoader::plugin PluginLoader::loadPlugin(std::string name, std::string type) {
    plugin result;
    result.refcount = 0;
    result.name = name;
    std::string library = "geblaat_" + type + "_" + name + ".dll";
    LOG_INFO("Loading %s", library.c_str());

    typedef PluginLoadable *(*newInstance_f)();
    newInstance_f newInstance;
    typedef void (*delInstance_f)(PluginLoadable *);
    delInstance_f delInstance;

    result.handle = LoadLibrary(TEXT(library.c_str()));
    if (result.handle == nullptr) {
        throw std::runtime_error(getWin32ErrorString());
    }

    newInstance = (newInstance_f)GetProcAddress((HMODULE)result.handle, "newInstance");
    if (!newInstance) {
        throw std::runtime_error(getWin32ErrorString());
    }

    delInstance = (delInstance_f)GetProcAddress((HMODULE)result.handle, "delInstance");
    if (!delInstance) {
        throw std::runtime_error(getWin32ErrorString());
    }

    int *test = (int *)GetProcAddress((HMODULE)result.handle, "test");
    if (test) {
        LOG_INFO("test exists, value %d", *test);
    } else {
        LOG_INFO("test does not exist");
    }

    geblaat_get_info_f geblaat_get_info = (geblaat_get_info_f)GetProcAddress((HMODULE)result.handle, "geblaat_get_info");

    if (geblaat_get_info) {
        LOG_INFO("geblaat_get_info exists, value %s", geblaat_get_info());
    } else {
        LOG_INFO("geblaat_get_info does not exist");
    }

    result.newInstance = newInstance;
    result.delInstance = delInstance;

    return result;
}

#else
// For now, Operating Systems that are not Microsoft Windows compatible
// are expected to be POSIX compatible. At the time of writing, no OS
// is considered not matching this requirement. Do you know any?

#include <dlfcn.h>

PluginLoader::plugin PluginLoader::loadPlugin(std::string name, std::string type) {
    plugin result;
    result.refcount = 0;
    result.name = name;
    std::string library = "libgeblaat_" + type + "_" + name + ".so";
    LOG_INFO("Loading %s", library.c_str());

    typedef PluginLoadable *(*newInstance_f)();
    newInstance_f newInstance;
    typedef void (*delInstance_f)(PluginLoadable *);
    delInstance_f delInstance;

    // For a BotModule to call into the BotClient, we need symbols
    // in global space.
    // Can Windows do this?
    // Also, this means we cannot support unloading modules on MUSL/Linux
    result.handle = dlopen(library.c_str(), RTLD_NOW | RTLD_GLOBAL);
    if (result.handle == nullptr) {
        throw std::runtime_error(dlerror());
    }

    int *test_c_api = (int *)dlsym(result.handle, "test_c_api");
    if (test_c_api) {

        set_botclient_f set_botclient = (set_botclient_f)dlsym(result.handle, "set_botclient");
        get_botmodule_f get_botmodule = (get_botmodule_f)dlsym(result.handle, "get_botmodule");
        if (set_botclient && get_botmodule) {
            result.newInstance = [set_botclient, get_botmodule]() { return new CAPI_BotModule(set_botclient, get_botmodule); };

            result.delInstance = [](PluginLoadable *me) {
                auto meme = dynamic_cast<CAPI_BotModule *>(me);
                if (meme)
                    delete meme;
            };
        }

    } else {
        newInstance = (newInstance_f)dlsym(result.handle, "newInstance");
        if (!newInstance) {
            throw std::runtime_error(dlerror());
        }

        delInstance = (delInstance_f)dlsym(result.handle, "delInstance");
        if (!delInstance) {
            throw std::runtime_error(dlerror());
        }

        result.newInstance = newInstance;
        result.delInstance = delInstance;
    }

    geblaat_get_info_f geblaat_get_info = (geblaat_get_info_f)dlsym(result.handle, "geblaat_get_info");

    if (geblaat_get_info) {
        LOG_INFO("geblaat_get_info exists, value %s", geblaat_get_info());
    } else {
        LOG_INFO("geblaat_get_info does not exist");
    }

    return result;
}
#endif
} // namespace geblaat
