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

#include "BotClient.hpp"

#include "../protocol/C2SProtocol.hpp"
#include "Connection.hpp"
#include "logger.hpp"

#include "BotModule.hpp"
#include "CAPI_BotModule.h"
#include "CAPI_BotModule.hpp"

#include "PluginLoader.hpp"

namespace geblaat {

BotClient::BotClient() {}

// PluginLoader::plugin BotClient::getCapiBotModule(void *handle) {
void BotClient::CapiBotModuleLoader(PluginLoader::Plugin &plugin) {
    //    set_botclient_f set_botclient = nullptr;
    //    get_botmodule_f get_botmodule = nullptr;
    new_botmodule_instance_f new_botmodule_instance = nullptr;
    del_botmodule_instance_f del_botmodule_instance = nullptr;
    const char *customError = nullptr;
    pluginloadable_t *plugin_info = nullptr;

    plugin.handle = pluginLoader->dlopen(plugin.type, plugin.name);
    if (!plugin.handle) {
        goto handleError;
    }

    plugin_info = (pluginloadable_t *)pluginLoader->dlsym(plugin.handle, "plugin_info");
    if (!plugin_info) {
        customError = "Not a geblaat-plugin";
        goto handleError;
    }

    if (plugin_info->abi.abi != pluginloadable_abi_c) {
        customError = "Not a geblaat C plugin";
        goto handleError;
    }

    if (plugin_info->abi.version != 1) {
        customError = "Incompatible version";
        goto handleError;
    }

    //    set_botclient = (set_botclient_f)pluginLoader->dlsym(plugin.handle, "set_botclient");
    //    get_botmodule = (get_botmodule_f)pluginLoader->dlsym(plugin.handle, "get_botmodule");
    //    if (set_botclient && get_botmodule) {
    //        plugin.newInstance = [set_botclient, get_botmodule]() { return new CAPI_BotModule(set_botclient, get_botmodule); };
    //        plugin.delInstance = [](PluginLoadable *me) { delete me; };
    //        return;
    //    }

    new_botmodule_instance = (new_botmodule_instance_f)pluginLoader->dlsym(plugin.handle, "new_botmodule_instance");
    del_botmodule_instance = (del_botmodule_instance_f)pluginLoader->dlsym(plugin.handle, "del_botmodule_instance");
    if (new_botmodule_instance && del_botmodule_instance) {
        plugin.newInstance = [new_botmodule_instance, del_botmodule_instance]() {
            return new CAPI_BotModule(new_botmodule_instance, del_botmodule_instance);
        };
        plugin.delInstance = [](PluginLoadable *me) { delete me; };
        return;
    }

handleError:
    if (plugin.handle)
        pluginLoader->dlclose(plugin.handle);
    plugin.handle = nullptr;
    plugin.newInstance = nullptr;
    plugin.delInstance = nullptr;
    if (customError)
        throw std::runtime_error(customError);
    throw std::runtime_error(pluginLoader->dlerror());
    return;
}

void BotClient::registerBotCommand(BotModule *mod, std::string command, OnCommand cmd) {
    if (mBotModules.contains(mod)) {
        auto prefix = mBotModules[mod];
        LOG_INFO("Registering Bot Command, Prefix %s, Command %s", prefix.c_str(), command.c_str());
        mCommands[prefix][command] = cmd;
    } else {
        LOG_ERROR("Bot Module Not Registered");
    }
}

nlohmann::json BotClient::getConfig(void) { return config; }

int BotClient::setConfig(const nlohmann::json &cfg) {
    try {
        config = cfg;

        if (!pluginLoader)
            throw new std::runtime_error("PluginLoader missing");

        pluginLoader->registerPluginLoader([this](PluginLoader::Plugin &plugin) { CapiBotModuleLoader(plugin); });

        auto networks = config["networks"];
        if (networks.is_array()) {
            for (auto &network : networks) {
                auto jsonProtocol = network["protocol"];
                mProtocol = dynamic_cast<C2SProtocol *>(pluginLoader->newInstance(jsonProtocol["name"], "protocol"));
                if (mProtocol) {
                    mProtocol->setClient(this);

                    auto modules = config["modules"];
                    if (modules.is_array()) {
                        for (auto &module : modules) {
                            BotModule *botModule =
                                dynamic_cast<BotModule *>(pluginLoader->newInstance(module["name"], "botmodule"));
                            if (!botModule) {
                                LOG_ERROR("Could not load Botmodule");
                                continue;
                            }
                            mBotModules[botModule] = module["prefix"];
                            botModule->setBotClient(this);
                            botModule->setConfig(module["config"]);
                        }
                    }

                    mProtocol->setConfig(jsonProtocol["config"]);

                } else {
                    std::string prototolName = jsonProtocol["name"];
                    LOG_ERROR("Cannot load protocol %s", prototolName.c_str());
                }
            }
        }
    } catch (nlohmann::json::exception &ex) {
        LOG_ERROR("JSON exception: %s", ex.what());
        return -1;
    } catch (std::exception &ex) {
        LOG_ERROR("Unknown exception: %s", ex.what());
        return -1;
    } catch (...) {
        LOG_ERROR("Unknown exception (not derived from std::exception)");
        return -1;
    }

    // Testing the command structure
    mCommands["!"]["blaat"] = [this](std::string command, std::string parameter, std::map<std::string, std::string> recvMessage) {
        LOG_INFO("command is %s", command.c_str());
        LOG_INFO("parameter is %s", parameter.c_str());

        std::map<std::string, std::string> sendMessage;

        sendMessage["type"] = "action";
        sendMessage["text/plain"] = "bææ";

        if (recvMessage["target/type"] == "channel") {
            sendMessage["target"] = recvMessage["target"];
        } else {
            sendMessage["target"] = recvMessage["sender"];
        }

        mProtocol->sendMessage(sendMessage);
    };

    return 0;
}

void BotClient::sendMessage(std::map<std::string, std::string> message) { mProtocol->sendMessage(message); }

BotClient::~BotClient() {
    // TODO Auto-generated destructor stub
    if (mProtocol)
        delete mProtocol;
}

void BotClient::onMessage(std::map<std::string, std::string> message) {
    if (message.contains("type")) {
        if (message["type"] == "message") {
            if (message.contains("text/plain")) {
                if (message["text/plain"].length()) {
                    for (auto &prefixedCommand : mCommands) {
                        auto prefix = prefixedCommand.first;

                        auto isbotcommand = message["text/plain"].find(prefix);
                        if (isbotcommand == 0) {
                            auto endbotcommand = message["text/plain"].find(" ");
                            std::string botcommand, params;
                            if (endbotcommand == std::string::npos) {
                                botcommand = message["text/plain"].substr(1);
                                params = "";
                            } else {
                                botcommand = message["text/plain"].substr(1, endbotcommand - 1);
                                params = message["text/plain"].substr(endbotcommand + 1);
                            }
                            if (prefixedCommand.second.contains(botcommand)) {
                                prefixedCommand.second[botcommand](botcommand, params, message);
                            }
                        }
                    }
                }
            }
        }
    }
}

} /* namespace geblaat */

#ifdef DYNAMIC_LIBRARY
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"

extern "C" {
[[gnu::cdecl]] geblaat::BotClient *newInstance(void) { return new geblaat::BotClient(); }
[[gnu::cdecl]] void delInstance(geblaat::BotClient *inst) { delete inst; }

pluginloadable_t plugin_info = {
    .name = "Bot Client",
    .description = "Bot functionality",
    .abi = {.abi = pluginloadable_abi_cpp, .version = 0},
};
}

#pragma GCC diagnostic pop
#endif
