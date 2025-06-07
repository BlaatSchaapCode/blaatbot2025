#pragma once

#include "Protocol.hpp"
#include <map>
#include <string>
#include <vector>

#include <uriParser.hpp>

#include "Connection.hpp"

#include "HTTP.hpp"

namespace geblaat {

// Order of the inheritance is important. PluginLoadable should
// come first otherwise casting the object will fail
class SimpleHTTP : public PluginLoadable, public HTTP {
    enum Method {
        GET,
        HEAD,
        OPTIONS,
        TRACE,
        PUT,
        DELETE,
        POST,
        PATCH,
        CONNECT,
    };

  public:
    SimpleHTTP() {};

    virtual void onData(std::vector<char> data) override;
    virtual void onConnected() override;
    virtual void onDisconnected() override;

    int get(std::string url, int version = 11) override;

    int setConfig(const nlohmann::json &cfg) {
        config = cfg;
        return 0;
    }
    nlohmann::json getConfig(void) override { return config; }

  private:
    nlohmann::json config;
    std::string getMethodString();
    uriparser::Uri *uri = nullptr;
    ;

    int version = {};
    Method method = {};
};

} // namespace geblaat
