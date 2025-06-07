/*

 Author:	André van Schoubroeck <andre@blaatschaap.be>
 License:	MIT

 SPDX-License-Identifier: MIT

 Copyright (c) 2025 André van Schoubroeck <andre@blaatschaap.be>

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

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
#include "PluginLoadable.hpp"
#include "protocol/Protocol.hpp"

namespace geblaat {
class Protocol;
class Connection : public PluginLoadable {

  public:
    virtual ~Connection();
    virtual void send(std::vector<char> data) = 0;
    void send(std::string s);
    virtual void sendLine(std::string s) { send(s + "\r\n"); }
    // void setProtocol(::geblaat::C2SProtocol *protocol);
    void setProtocol(Protocol *protocol);

    // re-introduced for things like http
    int setHostName(std::string hostName) {
        mHostName = hostName;
        return 0;
    }
    int setPort(uint16_t port) {
        mPort = port;
        return 0;
    }

    virtual int connect(void) { return -ENOSYS; }

    // Going to passing the configuration as a json, to allow for
    // different configuration options per connection type
    // without requiring all of them being in the API.
    virtual int setConfig(const nlohmann::json &) override { return -ENOSYS; }

  protected:
    Protocol *mProtocol;

    std::string mHostName;
    uint16_t mPort;
};

} // namespace geblaat
