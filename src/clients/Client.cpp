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
    mConnection->setProtocol(mIRC);
    mIRC->setConnection(mConnection);
    ((::network::cTlsConnection *)mConnection)->connect("irc.blaatschaap.be", 6697, true);

    //((::network::cTlsConnection *)mConnection)->connect("testnet.ergo.chat", 6697, true);

    // libtls dropped support for older protocols, I can't force it to use them if I try.
    // Even though the documentation said I could.
    //    ((::network::cTlsConnection *)mConnection)->connect("irc.chat4all.org", 6697, true, true);

    //    mConnection = new ::network::cTcpConnection();
    //    mConnection->setProtocol(mIRC);
    //    mIRC->setConnection(mConnection);
    //    // how to make this neat?
    //    ((::network::cTcpConnection*)mConnection)->connect("192.168.178.42");
}

Client::~Client() {
    // TODO Auto-generated destructor stub
}

} /* namespace client */
