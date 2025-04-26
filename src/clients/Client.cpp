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

#include <dlfcn.h>

namespace client {

::network::Connection *getConnection(std::string lib) {
    std::string library = "libblaatnet_" + lib + ".so";
    typedef ::network::Connection *(*fptr)();
    fptr func;

    void *handle = dlopen(library.c_str(), RTLD_LAZY | RTLD_GLOBAL);
    // void *handle = dlopen( library.c_str(), RTLD_NOW);
    if (handle == nullptr) {
        LOG_ERROR("%s", dlerror());
        return nullptr;
    }

    func = (fptr)dlsym(handle, "newInstance");
    if (!func) {
        LOG_ERROR("%s", dlerror());
        return nullptr;
    }
    return func();
}

Client::Client() {

    // auto irc = std::make_shared<protocol::cIRC>;
    mIRC = new protocol::IRC();
    mIRC->setNick("bb25");
    mIRC->setUser("bb25");
    mIRC->setRealName("BlaatBot2025");

    //    mConnection = new network::TlsConnection();
    //    mIRC->setConnection(mConnection);
    //    mConnection->setProtocol(mIRC);
    //    mConnection->setHostName("irc.blaatschaap.be");
    ////    mConnection->setPort(6697);
    //    mConnection->connect();

    //	mConnection = new ::network::cTcpConnection();
    //	mIRC->setConnection(mConnection);
    //	mConnection->setProtocol(mIRC);
    //	mConnection->setHostName("irc.blaatschaap.be");
    ////	mConnection->setPort(6667);
    //	mConnection->connect();

    mConnection = getConnection("tcp");

    if (mConnection) {
        LOG_DEBUG("Got connection of type %s", typeid(*mConnection).name());
        mIRC->setConnection(mConnection);
        mConnection->setProtocol(mIRC);
        mConnection->setHostName("irc.blaatschaap.be");
        mConnection->connect();
    }
}

Client::~Client() {
    // TODO Auto-generated destructor stub
}

} /* namespace client */
