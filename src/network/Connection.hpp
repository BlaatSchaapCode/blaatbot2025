/*
 * iconnection.hpp
 *
 *  Created on: 20 mrt. 2022
 *      Author: andre
 */

#pragma once

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include <cerrno>

#include "../protocol/Protocol.hpp"
namespace protocol {
class Protocol;
}

namespace network {

class cConnection {

  public:
    virtual ~cConnection();
    virtual void send(std::vector<char> data) = 0;
    void send(std::string s);
    void setProtocol(::protocol::Protocol *protocol);

    int setHostName(std::string hostName);
    int setPort(uint16_t port);

    virtual int setSecure(bool secure) { return -ENOTSUP; }
    virtual int setIgnoreInvalidCerficiate(bool ignoreInvalidCerficiate) { return -ENOTSUP; }
    virtual int setIgnoreInsecureProtocol(bool ignoreInsecureProtocol) { return -ENOTSUP; }

    virtual int connect(void) { return -ENOSYS; }

  protected:
    ::protocol::Protocol *mProtocol;
    std::string mHostName;
    uint16_t mPort;
};

} // namespace network
