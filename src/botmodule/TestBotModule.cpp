#include "TestBotModule.hpp"

#include <fstream>
#include <random>

#include "logger.hpp"
namespace geblaat {

TestBotModule::~TestBotModule() {}

std::string TestBotModule::getRandomQuote(void) {
    if (mQuotes.size()) {
        std::random_device r;
        std::default_random_engine e1(r());
        std::uniform_int_distribution<int> uniform_dist(0, mQuotes.size());
        int qid = uniform_dist(e1);
        LOG_INFO("Quote %d is %s", qid, mQuotes[qid].c_str());
        return mQuotes[qid];
    }
    return "Blaat in het kwadraat";
}

void TestBotModule::loadQuotes(void) {
    try {
        mQuotes.clear();
        std::ifstream quoteFile(quoteFileName);
        std::string quote;
        while (std::getline(quoteFile, quote)) {
            mQuotes.push_back(quote);
        }
        quoteFile.close();

    } catch (std::exception &ex) {
        LOG_ERROR("Error loading quotes: %s", ex.what());
    }
    LOG_INFO("There are %d quotes in the file", mQuotes.size());
}

int TestBotModule::setConfig(nlohmann::json config) {
    if (mBotClient) {
        mBotClient->registerBotCommand(
            this, "test", [this](std::string command, std::string parameters, std::map<std::string, std::string> message) {
                onTest(command, parameters, message);
            });

        mBotClient->registerBotCommand(
            this, "randomquote", [this](std::string command, std::string parameters, std::map<std::string, std::string> message) {
                onRandomQuote(command, parameters, message);
            });

        mBotClient->registerBotCommand(
            this, "loadquotes", [this](std::string command, std::string parameters, std::map<std::string, std::string> message) {
                onLoadQuotes(command, parameters, message);
            });

        if (config.contains("quotefile")) {
            if (config["quotefile"].is_string()) {
                quoteFileName = config["quotefile"];
                loadQuotes();
            }
        }

        return 0;
    }
    return -1;
}

void TestBotModule::onLoadQuotes(std::string command, std::string parameters, std::map<std::string, std::string> recvMessage) {
    std::map<std::string, std::string> sendMessage;
    sendMessage["type"] = "message";
    sendMessage["text/plain"] = "Re-loading quotes";

    if (recvMessage["target/type"] == "channel") {
        sendMessage["target"] = recvMessage["target"];
    } else {
        sendMessage["target"] = recvMessage["sender"];
    }

    sendMessage(sendMessage);
    loadQuotes();
}
void TestBotModule::onRandomQuote(std::string command, std::string parameters, std::map<std::string, std::string> recvMessage) {
    std::map<std::string, std::string> sendMessage;

    sendMessage["type"] = "message";
    sendMessage["text/plain"] = getRandomQuote();

    if (recvMessage["target/type"] == "channel") {
        sendMessage["target"] = recvMessage["target"];
    } else {
        sendMessage["target"] = recvMessage["sender"];
    }

    sendMessage(sendMessage);
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
