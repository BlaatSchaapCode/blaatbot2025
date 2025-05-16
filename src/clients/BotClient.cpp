/*
 * cClient.cpp
 *
 *  Created on: 14 mrt. 2025
 *      Author: andre
 */

#include "BotClient.hpp"

#include "Connection.hpp"
#include "Protocol.hpp"
#include "logger.hpp"

#include "PluginLoader.hpp"

namespace geblaat {

BotClient::BotClient() {}

//PluginLoader::plugin BotClient::getCapiBotModule(void* handle){
//	PluginLoader::plugin result = {};
//
//	return result;
//}


void BotClient::registerBotCommand(BotModule *mod, std::string command, OnCommand cmd) {
    if (mBotModules.contains(mod)) {
        auto prefix = mBotModules[mod];
        LOG_INFO("Registering Bot Command, Prefix %s, Command %s", prefix.c_str(), command.c_str());
        mCommands[prefix][command] = cmd;
    } else {
        LOG_ERROR("Bot Module Not Registered");
    }
}

int BotClient::setConfig(nlohmann::json config) {
    try {
        if (!pluginLoader)
            throw new std::runtime_error("PluginLoader missing");
        auto networks = config["networks"];
        if (networks.is_array()) {
            for (auto &network : networks) {
                auto jsonProtocol = network["protocol"];

                mProtocol = pluginLoader->newProtocol(jsonProtocol["type"]);
                if (mProtocol) {
                    mProtocol->setClient(this);

                    auto modules = config["modules"];
                    if (modules.is_array()) {
                        for (auto &module : modules) {
                            auto botModule = pluginLoader->newBotModule(module["type"]);
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
                    std::string prototolName = jsonProtocol["type"];
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
[[gnu::cdecl]] const char *geblaat_get_info(void) { return "blaat"; };
}

#pragma GCC diagnostic pop
#endif
