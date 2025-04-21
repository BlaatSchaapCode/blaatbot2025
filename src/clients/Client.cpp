/*
 * cClient.cpp
 *
 *  Created on: 14 mrt. 2025
 *      Author: andre
 */

#include "Client.hpp"

#include "../network/TcpConnection.hpp"
#include "../network/TlsConnection.hpp"
#include "../protocol/IRC.hpp"

namespace client {

Client::Client() {

    // auto irc = std::make_shared<protocol::cIRC>;
    mIRC = new protocol::IRC();
    mIRC->setNick("bb25");
    mIRC->setUser("bb25");
    mIRC->setRealName("BlaatBot2025");

    mConnection = new network::cTlsConnection();
    mIRC->setConnection(mConnection);
    mConnection->setProtocol(mIRC);
    mConnection->setHostName("irc.blaatschaap.be");
//    mConnection->setPort(6697);
    mConnection->connect();

//	mConnection = new ::network::cTcpConnection();
//	mIRC->setConnection(mConnection);
//	mConnection->setProtocol(mIRC);
//	mConnection->setHostName("irc.blaatschaap.be");
////	mConnection->setPort(6667);
//	mConnection->connect();


}

Client::~Client() {
    // TODO Auto-generated destructor stub
}

} /* namespace client */
