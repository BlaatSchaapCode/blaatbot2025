/*
 * PluginLoader.cpp
 *
 *  Created on: 26 apr. 2025
 *      Author: andre
 */

#include "PluginLoader.hpp"

#include <exception>

#include "utils/logger.hpp"

#include "protocol/IRC.hpp" // for the stub

namespace geblaat {

::geblaat::Client *PluginLoader::newClient(std::string type) {
    // stub
    return new ::geblaat::Client();
}

::geblaat::Protocol *PluginLoader::newProtocol(std::string type) {
    // stub
    if (type == "irc") {
        return new ::geblaat::IRC();
    }
    return nullptr;
}

::geblaat::Connection *PluginLoader::newConnection(std::string name) {
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

PluginLoader::networkPlugin PluginLoader::loadNetworkPlugin(std::string name) {
    networkPlugin result;
    result.refcount = 1;
    result.name = name;
    std::string library = "geblaat_network_" + name + ".dll";

    typedef ::geblaat::Connection *(*newInstance_f)();
    newInstance_f newInstance;
    typedef void (*delInstance_f)(::geblaat::Connection *);
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

    result.newConnection = newInstance;
    result.delConnection = delInstance;

    return result;
}

#else
// For now, Operating Systems that are not Microsoft Windows compatible
// are expected to be POSIX compatible. At the time of writing, no OS
// is considered not matching this requirement. Do you know any?

#include <dlfcn.h>

PluginLoader::networkPlugin PluginLoader::loadNetworkPlugin(std::string name) {
    networkPlugin result;
    result.refcount = 1;
    result.name = name;
    std::string library = "libgeblaat_network_" + name + ".so";

    typedef ::geblaat::Connection *(*newInstance_f)();
    newInstance_f newInstance;
    typedef void (*delInstance_f)(::geblaat::Connection *);
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

    int *test = (int *)dlsym(result.handle, "test");
    if (test) {
        LOG_INFO("test exists, value %d", *test);
    } else {
        LOG_INFO("test does not exist");
    }

    result.newConnection = newInstance;
    result.delConnection = delInstance;

    return result;
}
#endif
} // namespace geblaat
