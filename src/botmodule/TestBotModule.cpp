#include "TestBotModule.hpp"

namespace geblaat {

int TestBotModule::setConfig(nlohmann::json) {
    if (mBotClient) {
        mBotClient->registerBotCommand(
            this, "test", [this](std::string command, std::string parameters, std::map<std::string, std::string> message) {
                onTest(command, parameters, message);
            });
        return 0;
    }
    return -1;
}

void TestBotModule::onTest(std::string command, std::string parameters, std::map<std::string, std::string> recvMessage) {
    std::map<std::string, std::string> sendMessage;

    sendMessage["type"] = "message";
    sendMessage["text/plain"] = "1234";

    if (recvMessage["target/type"] == "channel") {
        sendMessage["target"] = recvMessage["target"];
    } else {
        sendMessage["target"] = recvMessage["sender"];
    }

    mBotClient->sendMessage(sendMessage);
}

} // namespace geblaat

#if defined DYNAMIC_LIBRARY
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"

extern "C" {
[[gnu::cdecl]] geblaat::TestBotModule *newInstance(void) { return new geblaat::TestBotModule(); }
[[gnu::cdecl]] void delInstance(geblaat::TestBotModule *inst) { delete inst; }
[[gnu::cdecl]] const char *geblaat_get_info(void) { return "blaat"; };
}

#pragma GCC diagnostic pop
#endif
