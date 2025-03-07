/*
 * iconnection.hpp
 *
 *  Created on: 20 mrt. 2022
 *      Author: andre
 */

#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace network {

class iConnection {
  public:
    virtual void send(std::vector<char> data) = 0;
    virtual void send(std::string) = 0;
    virtual void process(void) = 0;
};

} // namespace network
