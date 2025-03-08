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

namespace network {

class cConnection {
  public:
    cConnection(std::shared_ptr<protocol::cProtocol> protocol) { mProtocol = protocol; }
    virtual ~cConnection();
    virtual void send(std::vector<char> data) = 0;
    virtual void send(std::string) = 0;

  protected:
    std::shared_ptr<protocol::cProtocol> mProtocol;
};

} // namespace network
