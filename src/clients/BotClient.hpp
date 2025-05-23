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

// So this is my issue... once I include the PluginLoader here I get errors
// regarding BotClient not found in PluginLoader. A circular reference problem.
// Well.. I wish to handle CAPI botmodules inside BotClient rather then in the
// PluginLoader, that should be more of a generic thing.
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

    // PluginLoader::plugin getCapiBotModule(void *handle);
    void CapiBotModuleLoader(PluginLoader::Plugin &);

  protected:
    Protocol *mProtocol = nullptr;

  private:
    std::map<std::string, std::map<std::string, OnCommand>> mCommands;
    std::map<BotModule *, std::string> mBotModules;
};

} // namespace geblaat
