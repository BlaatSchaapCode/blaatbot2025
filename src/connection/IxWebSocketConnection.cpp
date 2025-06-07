#include "IxWebSocketConnection.hpp"
#include "utils/logger.hpp"

namespace geblaat {

IXWebSocketConnection::IXWebSocketConnection() { ix::initNetSystem(); }
IXWebSocketConnection::~IXWebSocketConnection() { ix::uninitNetSystem(); }

void IXWebSocketConnection::onMessageCallback(const ix::WebSocketMessagePtr &msg) {

    switch (msg->type) {
    case ix::WebSocketMessageType::Message:
        LOG_DEBUG(">>>: %s", msg->str.c_str());
        onData(msg->str);
        break;
    case ix::WebSocketMessageType::Open:
        LOG_INFO("Connected");
        if (mProtocol)
            mProtocol->onConnected();
        break;
    case ix::WebSocketMessageType::Close:
        LOG_INFO("Disconnected");
        if (mProtocol)
            mProtocol->onDisconnected();
        break;
    case ix::WebSocketMessageType::Error:
        break;
    case ix::WebSocketMessageType::Ping:
        break;
    case ix::WebSocketMessageType::Pong:
        break;
    case ix::WebSocketMessageType::Fragment:
        break;
    }
}

int IXWebSocketConnection::connect(void) {
    // We define two WebSocket subprotocols: binary.ircv3.net and text.ircv3.net.
    // TODO: get this from config
    webSocket.addSubProtocol("text.ircv3.net");

    // TODO get this from config
    webSocket.setUrl("wss://irc.blaatschaap.be:6443");

    webSocket.setOnMessageCallback([this](const ix::WebSocketMessagePtr &msg) { onMessageCallback(msg); });

    webSocket.start();

    return 0;
}
void IXWebSocketConnection::send(std::vector<char> data) {
    std::string string(data.begin(), data.end());
    LOG_DEBUG("<<<: %s", string.c_str());
    webSocket.send(string);
}

} // namespace geblaat
