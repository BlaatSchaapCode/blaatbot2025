#pragma once

#include "Connection.hpp"

namespace geblaat {
class WebSocketConnection : public Connection {
    // WebSocket IRC does not have \r\n at the end of a message
    // but uses WebSocket framing to indicate the end of a message.
    //
    // Here we override sendLine to have this behaviour on outgoing
    // messages, and implement onData for received messages.
    // Note we provide std::string and std::vector<char> implementations.
    // Internally we use std::vector<char> but a WebSocket implementation
    // like IXWebSocket uses std::string to store the received data.

  public:
    virtual void sendLine(std::string s) override { send(s); }

  protected:
    virtual void onData(std::vector<char> data) {
        data.emplace_back('\r');
        data.emplace_back('\n');
        if (mProtocol)
            mProtocol->onData(data);
    }
    virtual void onData(std::string data) {
        std::vector<char> v(data.begin(), data.end());
        onData(v);
    }
};

} // namespace geblaat
