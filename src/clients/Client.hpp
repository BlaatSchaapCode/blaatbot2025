#pragma once

// C++ Library Includes
#include <memory>


// Third Party libraries
#include <nlohmann/json.hpp>


// Project includes
#include "../protocol/IRC.hpp"
#include "../protocol/Protocol.hpp"



namespace client {

class Client {
  public:
    Client();
    virtual ~Client();
    int setConfig(nlohmann::json);
  protected:
    ::protocol::IRC *mIRC = nullptr;
    ::network::Connection *mConnection  = nullptr;

};

} /* namespace client */
