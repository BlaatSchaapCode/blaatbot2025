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



class Connection {
  public:
    virtual void send(std::vector<char> data) = 0;
    virtual void send(std::string) = 0;
    void process(std::vector<char> data);
    virtual ~Connection( );

  private:
    bool mLineMode = true;
    std::vector<char> mBuffer;
};

} // namespace network
