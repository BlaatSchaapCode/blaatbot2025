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

PluginLoader gPluginLoader; // testing

BotClient::BotClient() {}

int BotClient::setConfig(nlohmann::json config) {
    try {
        auto networks = config["networks"];
        if (networks.is_array()) {
            for (auto &network : networks) {
                auto jsonProtocol = network["protocol"];

                mProtocol = gPluginLoader.newProtocol(jsonProtocol["type"]);
                if (mProtocol) {
                    mProtocol->setClient(this);
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
        // m["target"] = message["sender"];
        if (recvMessage["target/type"] == "channel") {
            sendMessage["target"] = recvMessage["target"];
        } else {
            sendMessage["target"] = recvMessage["sender"];
        }

        mProtocol->sendMessage(sendMessage);
    };

    return 0;
}

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
