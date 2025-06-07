#pragma once

#include <string>

#include "C2SProtocol.hpp"

namespace geblaat {
class HTTP : public C2SProtocol {
public:
    virtual int get(std::string url, int version = 11) = 0;

    int setConfig(const nlohmann::json &) override { return -1; }
    nlohmann::json getConfig(void) override {
        nlohmann::json result;
        return result;
    }
    void sendMessage(std::map<std::string, std::string> message) override {}
};
} // namespace geblaat
