#pragma once

#include "BotModule.hpp"
#include "CAPI_BotModule.h"
#include <map>
#include <string>
namespace geblaat {

class CAPI_BotModule : public BotModule {
  public:
    int setConfig(nlohmann::json) override;

    CAPI_BotModule(set_botclient_f, get_botmodule_f);
    virtual ~CAPI_BotModule() {}
    void registerBotCommand(const char *command, on_bot_command_callback_f handler);

  private:
    std::map<std::string, on_bot_command_callback_f> botCommands;
    void onBotCommand(std::string command, std::string parameters, std::map<std::string, std::string> recvMessage);

    botmodule_c_api_t *botModule = nullptr;
    ;
    botclient_c_api_t botClient = {
        .client = this,
        .register_bot_command =
            [](const void *s, const char *command, on_bot_command_callback_f handler) {
                CAPI_BotModule *self = (CAPI_BotModule *)s;
                self->registerBotCommand(command, handler);
                return 0;
            },
        .send_message =
            [](const void *s, const key_value_t *kv) {
                CAPI_BotModule *self = (CAPI_BotModule *)s;
                std::map<std::string, std::string> message;
                while (kv->key) {
                    message[kv->key] = kv->value;
                    kv++;
                }
                self->mBotClient->sendMessage(message);
                return 0;
            },

    };
};
} // namespace geblaat
