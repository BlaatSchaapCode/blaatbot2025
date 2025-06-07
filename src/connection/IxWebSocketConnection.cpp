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
        // msg->openInfo.headers; // its a map.
        LOG_INFO("Connected to uri %s using protocol %s", msg->openInfo.uri.c_str(), msg->openInfo.protocol.c_str());
        if (mProtocol)
            mProtocol->onConnected();
        break;
    case ix::WebSocketMessageType::Close:
        LOG_INFO("Disconnected: code %04X reason: %s remotr: %d", msg->closeInfo.code, msg->closeInfo.reason.c_str(),
                 msg->closeInfo.remote);
        if (mProtocol)
            mProtocol->onDisconnected();
        break;
    case ix::WebSocketMessageType::Error:
        // Report error to client?
        // msg->errorInfo;
        // uint32_t retries = 0;
        // double wait_time = 0;
        // int http_status = 0;
        // std::string reason;
        // bool decompressionError = false;
        break;
    case ix::WebSocketMessageType::Ping:
        //  msg->str
        break;
    case ix::WebSocketMessageType::Pong:
        //  msg->str
        break;
    case ix::WebSocketMessageType::Fragment:
        // ??

        break;
    }
}

int IXWebSocketConnection::connect(void) {
    // We define two WebSocket subprotocols: binary.ircv3.net and text.ircv3.net.
    // TODO: get this from config
    webSocket.addSubProtocol("text.ircv3.net");

    // TODO get this from config
    //
    // Proposed to support both uri and regular hostname/port forms
    // For hostname/port use we also need an additional plain/tls flag.
    // Does websocket also allow a path? Seems the onConnect event
    // reports an uri of "/", suggesting this is the path part of the uri?

    // Also look at the options to ignore invalid certificated
    // Seems 'disable_hostname_validation' is the only field in
    // SocketTLSOptions related to this? No ignore expired option?
    // webSocket.setTLSOptions();

    // Also, it seems by default it automatically reconnects on
    // disconnect. We should probably disable this behaviour and
    // make the Client steer the reconnection behaviour.

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

#ifdef DYNAMIC_LIBRARY
extern "C" {
geblaat::IXWebSocketConnection *newInstance(void) { return new geblaat::IXWebSocketConnection(); }
void delInstance(geblaat::IXWebSocketConnection *inst) { delete inst; }
pluginloadable_t plugin_info = {
    .name = "IxWebSocketConnection",
    .description = "WebSocket Connection support using IxWebSocket",
    .abi = {.abi = pluginloadable_abi_cpp, .version = 0},
};
}
#endif
