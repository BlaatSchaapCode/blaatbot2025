/*
 * iconnection.hpp
 *
 *  Created on: 20 mrt. 2022
 *      Author: andre
 */

#pragma once

// C++ library
#include <memory>
#include <string>
#include <vector>

// C library
#include <cerrno>
#include <cstdint>

// Third Party libraries
#include <nlohmann/json.hpp>

// Project includes
#include "../protocol/Protocol.hpp"
#include "PluginLoadable.hpp"
namespace geblaat {
class Protocol;
}

namespace geblaat {

class Connection : public PluginLoadable {

  public:
    virtual ~Connection();
    virtual void send(std::vector<char> data) = 0;
    void send(std::string s);
    void setProtocol(::geblaat::Protocol *protocol);

    //		Replaced with setConfig
    //    int setHostName(std::string hostName);
    //    int setPort(uint16_t port);
    //
    //    virtual int setSecure(bool secure) { return -ENOTSUP; }
    //    virtual int setIgnoreInvalidCerficiate(bool ignoreInvalidCerficiate) { return -ENOTSUP; }
    //    virtual int setIgnoreInsecureProtocol(bool ignoreInsecureProtocol) { return -ENOTSUP; }

    virtual int connect(void) { return -ENOSYS; }

    // Going to passing the configuration as a json, to allow for
    // different configuration options per connection type
    // without requiring all of them being in the API.
    virtual int setConfig(nlohmann::json) { return -ENOSYS; }

  protected:
    ::geblaat::Protocol *mProtocol;
    std::string mHostName;
    uint16_t mPort;
};

} // namespace geblaat
