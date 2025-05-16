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

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace geblaat {

PluginLoadable *PluginLoader::newInstance(std::string name, std::string type) {
    if (!this->plugins.contains(type + "_" + name)) {
        try {
            this->plugins[type + "_" + name] = loadPlugin(name, type);
        } catch (std::exception &ex) {
            LOG_ERROR("Error: %s", ex.what());
        }
    }
    if (this->plugins.contains(type + "_" + name)) {
        auto instance = this->plugins[type + "_" + name].newInstance();
        LOG_INFO("Got an instance of %s", demangleClassName(typeid(instance).name()).c_str());
        this->plugins[type + "_" + name].refcount++;
        instance->setPluginLoader(this);
        return instance;

    } else {
        LOG_ERROR("Error: Plugin not found");
    }

    return nullptr;
}

#if defined(_WIN32) || defined(_WIN64)

void *PluginLoader::dlsym(void *handle, const char *symbol) { return (void *)GetProcAddress((HMODULE)handle, symbol); }
void *PluginLoader::dlopen(std::string type, std::string name) {
    std::string library = "geblaat_" + type + "_" + name + ".dll";
    return LoadLibrary(TEXT(library.c_str()));
}
int PluginLoader::dlclose(void *handle) { return !FreeLibrary((HMODULE)handle); }
std::string PluginLoader::dlerror(void) {
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

#else
// For now, Operating Systems that are not Microsoft Windows compatible
// are expected to be POSIX compatible. At the time of writing, no OS
// is considered not matching this requirement. Do you know any?

void *PluginLoader::dlsym(void *handle, const char *symbol) { return ::dlsym(handle, symbol); }
void *PluginLoader::dlopen(std::string type, std::string name) {
    std::string library = "libgeblaat_" + type + "_" + name + ".so";
    return ::dlopen(library.c_str(), RTLD_NOW | RTLD_LOCAL);
}
int PluginLoader::dlclose(void *handle) { return ::dlclose(handle); }
std::string PluginLoader::dlerror(void) { return ::dlerror(); }
#endif

PluginLoader::plugin PluginLoader::loadPlugin(std::string name, std::string type) {
    plugin result;
    result.refcount = 0;
    result.name = name;
    typedef PluginLoadable *(*newInstance_f)();
    newInstance_f newInstance;
    typedef void (*delInstance_f)(PluginLoadable *);
    delInstance_f delInstance;

    result.handle = dlopen(type, name);
    if (result.handle == nullptr) {
        throw std::runtime_error(dlerror());
    }

    int *test_c_api = (int *)dlsym(result.handle, "test_c_api");
    if (test_c_api) {
        // I have the C API loader for botmodule into BotClient
        // Now I need to access it from here. The BotClient
        // has to register module loaders here for APIs.

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

} // namespace geblaat
