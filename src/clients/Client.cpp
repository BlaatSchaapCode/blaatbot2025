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

Client::Client() {

    //    // auto irc = std::make_shared<protocol::cIRC>;
    //    mIRC = new protocol::IRC();
    //    mIRC->setNick("bb25");
    //    mIRC->setUser("bb25");
    //    mIRC->setRealName("BlaatBot2025");
    //
    //    //    mConnection = new network::TlsConnection();
    //    //    mIRC->setConnection(mConnection);
    //    //    mConnection->setProtocol(mIRC);
    //    //    mConnection->setHostName("irc.blaatschaap.be");
    //    ////    mConnection->setPort(6697);
    //    //    mConnection->connect();
    //
    //    //	mConnection = new ::network::cTcpConnection();
    //    //	mIRC->setConnection(mConnection);
    //    //	mConnection->setProtocol(mIRC);
    //    //	mConnection->setHostName("irc.blaatschaap.be");
    //    ////	mConnection->setPort(6667);
    //    //	mConnection->connect();
    //
    //    //  mConnection = getConnection("tcp");
    //
    //    mConnection = gPluginLoader.newConnection("tcp");
    //
    //    if (mConnection) {
    //        LOG_DEBUG("Got connection of type %s", typeid(*mConnection).name());
    //        mIRC->setConnection(mConnection);
    //        mConnection->setProtocol(mIRC);
    //        mConnection->setHostName("irc.blaatschaap.be");
    //        mConnection->connect();
    //    }
}

int Client::setConfig(nlohmann::json config) {
    try {
        auto networks = config["networks"];
        if (networks.is_array()) {
            for (auto &network : networks) {
                auto jsonProtocol = network["protocol"];

                auto protocol = gPluginLoader.newProtocol(jsonProtocol["type"]);
                if (protocol) {
                    protocol->setConfig(jsonProtocol["config"]);
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
}

} /* namespace client */
