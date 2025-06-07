/*

 Author:	André van Schoubroeck <andre@blaatschaap.be>
 License:	MIT

 SPDX-License-Identifier: MIT

 Copyright (c) 2025 André van Schoubroeck <andre@blaatschaap.be>

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 */

#include "PluginLoader.hpp"
#include "executablePath.hpp"

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

PluginLoader::PluginLoader() {
    LOG_INFO("Executable path:  %s", getExecutablePath().c_str());
    LOG_INFO("Plugin directory: %s", getPluginDir().c_str());

    pluginDirectory = getPluginDir(); // TODO: Allow for settings to override this
    registerPluginLoader([this](Plugin &plugin) { loadCppPlugin(plugin); });
}

void PluginLoader::registerPluginLoader(Loader loader) { loaders.push_back(loader); }

void PluginLoader::loadCppPlugin(Plugin &plugin) {
    typedef PluginLoadable *(*newInstance_f)();
    newInstance_f newInstance;
    typedef void (*delInstance_f)(PluginLoadable *);
    delInstance_f delInstance;
    const char *customError = nullptr;
    pluginloadable_t *plugin_info = nullptr;

    plugin.handle = dlopen(plugin.type, plugin.name);
    if (!plugin.handle) {
        goto handleError;
    }

    plugin_info = (pluginloadable_t *)dlsym(plugin.handle, "plugin_info");
    if (!plugin_info) {
        customError = "Not a geblaat-plugin";
        goto handleError;
    }

    if (plugin_info->abi.abi != pluginloadable_abi_cpp) {
        customError = "Not a geblaat C++ plugin";
        goto handleError;
    }

    if (plugin_info->abi.version != 0) {
        customError = "Incompatible version";
        goto handleError;
    }

    newInstance = (newInstance_f)dlsym(plugin.handle, "newInstance");
    if (!newInstance) {
        goto handleError;
    }

    delInstance = (delInstance_f)dlsym(plugin.handle, "delInstance");
    if (!delInstance) {
        goto handleError;
    }
    plugin.newInstance = newInstance;
    plugin.delInstance = delInstance;
    return;
handleError:
    if (plugin.handle)
        dlclose(plugin.handle);
    plugin.handle = nullptr;
    plugin.newInstance = nullptr;
    plugin.delInstance = nullptr;
    if (customError)
        throw std::runtime_error(customError);
    throw std::runtime_error(dlerror());
    return;
}

PluginLoadable *PluginLoader::newInstance(std::string name, std::string type) {
    if (!this->plugins.contains(type + "_" + name)) {
        try {
            this->plugins[type + "_" + name] = loadPlugin(name, type);
        } catch (std::exception &ex) {
            LOG_ERROR("Error loading %s %s: %s", name.c_str(), type.c_str(), ex.what());
        }
    }
    if (this->plugins.contains(type + "_" + name)) {
        auto instance = this->plugins[type + "_" + name].newInstance();
        if (instance) {
            LOG_INFO("Got an instance of %s", demangleClassName(typeid(instance).name()).c_str());
            this->plugins[type + "_" + name].refcount++;
            instance->setPluginLoader(this);
        } else {
            LOG_ERROR("Failed to get an instance");
        }
        return instance;
    } else {
        LOG_ERROR("Error loading %s %s: %s", name.c_str(), type.c_str(), "Plugin not found");
    }

    return nullptr;
}

PluginLoader::Plugin PluginLoader::loadPlugin(std::string name, std::string type) {
    Plugin result = {};
    result.name = name;
    result.type = type;

    std::string lastError = "Plugin not found";

    for (Loader &loader : loaders) {
        try {
            loader(result);
            return result;
        } catch (std::exception &ex) {
            // Look into error handling
            // keep using  Exceptions or use something else
            // Do we wnat to return not found, or look into them more
            lastError = ex.what();
            continue;
        }
    }

    throw std::runtime_error(lastError);
}

///-------------
#if defined(_WIN32) || defined(_WIN64)

void *PluginLoader::dlsym(void *handle, const char *symbol) { return (void *)GetProcAddress((HMODULE)handle, symbol); }
void *PluginLoader::dlopen(std::string type, std::string name) {
    std::string library = pluginDirectory + "\\geblaat_" + type + "_" + name + ".dll";
    return LoadLibrary(TEXT(library.c_str()));
}
int PluginLoader::dlclose(void *handle) { return !FreeLibrary((HMODULE)handle); }
std::string PluginLoader::dlerror(void) {
    // https://stackoverflow.com/questions/1387064/how-to-get-the-error-message-from-the-error-code-returned-by-getlasterror
    DWORD errorMessageID = GetLastError();
    LPSTR messageBuffer = nullptr;

    // Ask Win32 to give us the string version of that message ID.
    // The parameters we pass in, tell Win32 to create the buffer that holds the
    // message for us (because we don't yet know how long the message string will
    // be).
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
                                 errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    // Copy the error message into a std::string.
    std::string message(messageBuffer, size);

    // Free the Win32's string's buffer.
    LocalFree(messageBuffer);
    return message;
}

#else
// For now, Operating Systems that do not implement the WIN32 API
// are expected to implement the POSIX API.

#if defined __ELF__    // Platform uses the ELF binary format
constexpr std::string library_suffix = ".so";
#elif defined __MACH__ // Platform uses the Mach-O binary format
constexpr std::string library_suffix = ".dlsym";
#else
#error "Cannot determine the platform binary format"
#endif

void *PluginLoader::dlsym(void *handle, const char *symbol) { return ::dlsym(handle, symbol); }
void *PluginLoader::dlopen(std::string type, std::string name) {
    std::string library = pluginDirectory + "/libgeblaat_" + type + "_" + name + library_suffix;
    return ::dlopen(library.c_str(), RTLD_NOW | RTLD_LOCAL);
}
int PluginLoader::dlclose(void *handle) { return ::dlclose(handle); }
std::string PluginLoader::dlerror(void) { return ::dlerror(); }
#endif
//--------------

} // namespace geblaat
