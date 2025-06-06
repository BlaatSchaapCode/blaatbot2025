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
    std::string pluginDirectory;
};

}; // namespace geblaat
#endif /* SRC_PLUGINLOADER_HPP_ */
