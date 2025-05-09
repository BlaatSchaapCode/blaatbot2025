/*
 * cClient.cpp
 *
 *  Created on: 14 mrt. 2025
 *      Author: andre
 */

#include "Client.hpp"

#include "../network/Connection.hpp"
#include "../protocol/IRC.hpp"
#include "../utils/logger.hpp"

#include "PluginLoader.hpp"

namespace geblaat {

extern PluginLoader gPluginLoader; // testing

Client::Client() {}

int Client::setConfig(nlohmann::json config) {
    try {
        auto networks = config["networks"];
        if (networks.is_array()) {
            for (auto &network : networks) {
                auto jsonProtocol = network["protocol"];

                mProtocol = gPluginLoader.newProtocol(jsonProtocol["type"]);
                if (mProtocol) {
                    mProtocol->setClient(this);
                    mProtocol->setConfig(jsonProtocol["config"]);
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
    return 0;
}

Client::~Client() {
    // TODO Auto-generated destructor stub
    if (mProtocol)
        delete mProtocol;
}

void Client::onMessage(std::map<std::string, std::string> message) {
    std::string prefix = "!";

    //    LOG_INFO("Message:");
    //    for (auto &m : message) {
    //        LOG_INFO("%24s : %s", m.first.c_str(), m.second.c_str());
    //    }

    if (message.contains("type")) {
        if (message["type"] == "message") {
            if (message.contains("text/plain")) {
                if (message["text/plain"].length()) {
                    auto isbotcommand = message["text/plain"].find(prefix);
                    if (isbotcommand == 0) {
                        auto endbotcommand = message["text/plain"].find(" ");
                        std::string botcommand;
                        if (endbotcommand == std::string::npos) {
                            botcommand = message["text/plain"].substr(1);
                        } else {
                            botcommand = message["text/plain"].substr(1, endbotcommand - 1);
                        }
                        LOG_INFO("Botcommand %s", botcommand.c_str());
                        if (botcommand == "blaat") {
                            std::map<std::string, std::string> m;
                            m["type"] = "action";
                            m["text/plain"] = "bææ";
                            // m["target"] = message["sender"];
                            if (message["target/type"] == "channel") {
                                m["target"] = message["target"];
                            } else {
                                m["target"] = message["sender"];
                            }
                            mProtocol->sendMessage(m);
                        }
                    }
                }
            }
        }
    }
}

} /* namespace geblaat */
