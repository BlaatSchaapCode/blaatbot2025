#pragma once

#include <memory>

#include "../protocol/IRC.hpp"
#include "../protocol/Protocol.hpp"
#include "../PluginLoader.hpp"
namespace client {

class Client {
  public:
    Client();
    virtual ~Client();
  protected:
    ::protocol::IRC *mIRC;
    ::network::Connection *mConnection;
  private:
    PluginLoader pl;
};

} /* namespace client */
