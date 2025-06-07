#include "Connection.hpp"
#include "SimpleHTTP.hpp"
#include "PluginLoader.hpp"
#include "utils/logger.hpp"


namespace geblaat {

std::string SimpleHTTP::getMethodString() {
    switch (method) {
    case GET:
        return "GET";
    case HEAD:
        return "HEAD";
    case OPTIONS:
        return "OPTIONS";
    case TRACE:
        return "TRACE";
    case PUT:
        return "PUT";
    case DELETE:
        return "DELETE";
    case POST:
        return "POST";
    case PATCH:
        return "PATCH";
    case CONNECT:
        return "CONNECT";
    default:
        return "UNKNOWN";
    }
}

void SimpleHTTP::onData(std::vector<char> data) {
	LOG_INFO("Got a response from the server: %d bytes", data.size());

	// Parse headers, determine when we are done
	// Implement timeouts as needed for matrix

	// Cannot delete the connection here, as this callback is
	// being called from the receive thread.
	//delete mConnection;
	//mConnection = nullptr;
}
void SimpleHTTP::onConnected() {
    switch (version) {
    case 9:
        // HTTP 0.9
        mConnection->sendLine(getMethodString() + " " + uri->path() +
        		(uri->query().length() ? "?" : "") + uri->query() );
        mConnection->sendLine("");
        break;
    case 10:
        // HTTP 1.0
    	mConnection->sendLine(getMethodString() + " " + uri->path() +
    	        		(uri->query().length() ? "?" : "") + uri->query() +" HTTP/1.0");
		mConnection->sendLine("");
        break;
    case 11:
    default:
        // HTTP 1.1
    	mConnection->sendLine(getMethodString() + " " + uri->path() +
    	        		(uri->query().length() ? "?" : "") + uri->query() +" HTTP/1.1");
    	mConnection->sendLine("Host: " + uri->host());
    	mConnection->sendLine("");
        break;
    }
}
void SimpleHTTP::onDisconnected() {}

int SimpleHTTP::get(std::string url, int ver) {
	if (mConnection) {
	        // Busy
	        return -2;
	    }

	version = ver;
    uri = new uriparser::Uri(url);
    if (! uri->isValid() ) {
        delete uri;
        uri = nullptr;
    	return -1;
    }

    uint16_t port = atoi(uri->port().c_str());

    if ( uri->scheme() == "http") {
        if (!port)
            port = 80;
        mConnection =  dynamic_cast<geblaat::Connection *>(pluginLoader->newInstance("tcp", "connection"));
    } else if (uri->scheme() == "https") {
        if (!port)
            port = 443;
        // Todo: have a "provides tls" in the plugin so we can have any here
        // mConnection =  dynamic_cast<geblaat::Connection *>(pluginLoader->newInstance("libretls", "connection"));
        mConnection =  dynamic_cast<geblaat::Connection *>(pluginLoader->newInstance("gnutls", "connection"));
    } else {
        LOG_ERROR("Unsupported scheme %s" , uri->scheme().c_str());
        delete uri;
        uri = nullptr;
    	return -2;
    }

    if (!mConnection) {
        delete uri;
        uri = nullptr;
        return -3;
    }


    mConnection->setProtocol(this);
    mConnection->setHostName(uri->host());
    mConnection->setPort(port);
    mConnection->connect();

    return 0;
}

} // namespace geblaat

#ifdef DYNAMIC_LIBRARY
extern "C" {
geblaat::SimpleHTTP *newInstance(void) { return new geblaat::SimpleHTTP(); }
void delInstance(geblaat::SimpleHTTP *inst) { delete inst; }
pluginloadable_t plugin_info = {
    .name = "SimpleHTTP",
    .description = "HTTP Protocol support",
    .abi = {.abi = pluginloadable_abi_cpp, .version = 0},
};
}
#endif
