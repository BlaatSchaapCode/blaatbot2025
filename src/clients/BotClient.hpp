#pragma once

// C++ Library Includes
#include <functional>
#include <map>
#include <memory>
#include <string>

// Third Party libraries
#include <nlohmann/json.hpp>

// Project includes

#include "Client.hpp"
#include "PluginLoader.hpp"

namespace geblaat {
class BotModule;
class Protocol;
class BotClient : public Client {
  public:
    BotClient();
    virtual ~BotClient();
    int setConfig(nlohmann::json) override;
    void onMessage(std::map<std::string, std::string> message) override;

    using OnCommand = std::function<void(std::string command, std::string parameters, std::map<std::string, std::string> message)>;

    void registerBotCommand(BotModule *mod, std::string command, OnCommand cmd);
    void sendMessage(std::map<std::string, std::string> message);

    //PluginLoader::plugin getCapiBotModule(void* handle);

  protected:
    Protocol *mProtocol = nullptr;

  private:
    std::map<std::string, std::map<std::string, OnCommand>> mCommands;
    std::map<BotModule *, std::string> mBotModules;
};

} // namespace geblaat
