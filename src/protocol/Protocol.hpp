#pragma once

#include "connection/Connection.hpp"

namespace geblaat {
class Connection;
class Protocol {
  public:
    virtual ~Protocol() {};
    virtual void onData(std::vector<char> data) = 0;
    virtual void onConnected() = 0;
    virtual void onDisconnected() = 0;
    void setConnection(Connection *connection) { mConnection = connection; }

  protected:
    Connection *mConnection = nullptr;
};

} // namespace geblaat
