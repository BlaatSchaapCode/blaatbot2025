#pragma once

#include "C2SProtocol.hpp"
#include <map>
#include <string>
#include <vector>

#include <uriParser.hpp>

#include "Connection.hpp"

#include "HTTP.hpp"

namespace geblaat {

class SimpleHTTP : public HTTP {
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

  private:
    std::string getMethodString();
    uriparser::Uri * uri = nullptr;;

    int version = {};
    Method method = {};
};

} // namespace geblaat
