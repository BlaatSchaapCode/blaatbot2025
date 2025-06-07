#pragma once

#include "Connection.hpp"

namespace geblaat {
class WebSocketConnection : public Connection {
  public:
    virtual void sendLine(std::string s) override {send(s);}

};

} // namespace geblaat
