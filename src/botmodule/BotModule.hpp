#pragma once

#include "PluginLoadable.hpp"
#include "clients/BotClient.hpp"

namespace geblaat {

class BotModule : public PluginLoadable {
  public:
    void setBotClient(BotClient *botclient) { mBotClient = botclient; }

  protected:
    BotClient *mBotClient = nullptr;
};
} // namespace geblaat
