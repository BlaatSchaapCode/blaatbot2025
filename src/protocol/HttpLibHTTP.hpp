#pragma once

#include "Protocol.hpp"
#include <map>
#include <string>
#include <vector>

#include <uriParser.hpp>

#define  CPPHTTPLIB_OPENSSL_SUPPORT
#include <httplib.h>

#include "Connection.hpp"

#include "HTTP.hpp"

namespace geblaat {

class HttpLibHTTP :  public PluginLoadable, public HTTP {
public:
    int get(std::string url, int version = 11) override;


//    // These are not applicable, might need another refactor
    void onData(std::vector<char> data) override  {};
    void onConnected() override  {};
    void onDisconnected() override {};

    int setConfig(const nlohmann::json &cfg) {
        config = cfg;
        return 0;
    }
    nlohmann::json getConfig(void) override { return config; }

  private:
    nlohmann::json config;

private:

};
} // namespace geblaat
