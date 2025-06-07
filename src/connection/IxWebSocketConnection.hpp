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


