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

#pragma once

// C++ Library Includes
#include <functional>
#include <map>
#include <memory>
#include <string>

// Third Party libraries
#include <nlohmann/json.hpp>

// Project includes

#include "Client.hpp"

// So this is my issue... once I include the PluginLoader here I get errors
// regarding BotClient not found in PluginLoader. A circular reference problem.
// Well.. I wish to handle CAPI botmodules inside BotClient rather then in the
// PluginLoader, that should be more of a generic thing.
#include "PluginLoader.hpp"

namespace geblaat {
class BotModule;
class Protocol;
class BotClient : public Client {
  public:
    BotClient();
    virtual ~BotClient();
    int setConfig(nlohmann::json) override;
    void onMessage(std::map<std::string, std::string> message) override;

    using OnCommand = std::function<void(std::string command, std::string parameters, std::map<std::string, std::string> message)>;

    void registerBotCommand(BotModule *mod, std::string command, OnCommand cmd);
    void sendMessage(std::map<std::string, std::string> message);

    // PluginLoader::plugin getCapiBotModule(void *handle);
    void CapiBotModuleLoader(PluginLoader::Plugin &);

  protected:
    Protocol *mProtocol = nullptr;

  private:
    std::map<std::string, std::map<std::string, OnCommand>> mCommands;
    std::map<BotModule *, std::string> mBotModules;
};

} // namespace geblaat
