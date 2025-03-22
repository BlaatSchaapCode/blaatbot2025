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

#include "cProtocol.hpp"

namespace protocol {
class cProtocol;
}

namespace network {

class cConnection : public std::enable_shared_from_this<cConnection> {

  public:
    virtual ~cConnection();
    virtual void send(std::vector<char> data) = 0;
    void send(std::string s) {
        std::vector<char> v(s.begin(), s.end());
        send(v);
    }
    void setProtocol(std::shared_ptr<::protocol::cProtocol> protocol) { mProtocol = protocol; }

  protected:
    std::shared_ptr<::protocol::cProtocol> mProtocol;
};

} // namespace network
