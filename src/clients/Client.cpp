/*
 * cClient.cpp
 *
 *  Created on: 14 mrt. 2025
 *      Author: andre
 */

#include "Client.hpp"

// #include "../network/TcpConnection.hpp"
// #include "../network/TlsConnection.hpp"
#include "../network/Connection.hpp"
#include "../protocol/IRC.hpp"
#include "../utils/logger.hpp"

#include "PluginLoader.hpp"
extern PluginLoader gPluginLoader; // testing

namespace client {

Client::Client() {}

int Client::setConfig(nlohmann::json config) {
    try {
        auto networks = config["networks"];
        if (networks.is_array()) {
            for (auto &network : networks) {
                auto jsonProtocol = network["protocol"];

                mProtocol = gPluginLoader.newProtocol(jsonProtocol["type"]);
                if (mProtocol) {
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

} /* namespace client */
