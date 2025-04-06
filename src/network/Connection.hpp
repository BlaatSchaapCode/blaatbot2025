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

#include "../protocol/Protocol.hpp"

namespace protocol {
class Protocol;
}

namespace network {

class cConnection {

  public:
    virtual ~cConnection();
    virtual void send(std::vector<char> data) = 0;
    void send(std::string s) {
        std::vector<char> v(s.begin(), s.end());
        send(v);
    }
    void setProtocol(::protocol::Protocol *protocol) { mProtocol = protocol; }

  protected:
    ::protocol::Protocol *mProtocol;
};

} // namespace network
