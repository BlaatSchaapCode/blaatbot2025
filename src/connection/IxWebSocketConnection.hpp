#pragma once

#include "WebSocketConnection.hpp"

#include <ixwebsocket/IXNetSystem.h>
#include <ixwebsocket/IXUserAgent.h>
#include <ixwebsocket/IXWebSocket.h>



namespace geblaat{

class IXWebSocketConnection : public WebSocketConnection {
public:
	IXWebSocketConnection();
	~IXWebSocketConnection();
	int connect(void) override;
	virtual void send(std::vector<char> data) override;
private:
	ix::WebSocket webSocket;
	void onMessageCallback(const ix::WebSocketMessagePtr& msg);
};
}

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
