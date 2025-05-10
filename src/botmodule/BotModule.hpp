#pragma once

#include "clients/BotClient.hpp"
#include "PluginLoadable.hpp"

namespace geblaat {
class BotModule : public PluginLoadable {
  public:
    void setBotClient(BotClient *botclient) { mBotClient = botclient; }
  protected:
    BotClient *mBotClient = nullptr;
};
} // namespace geblaat
