#include "CAPI_BotModule.hpp"
#include "CAPI_BotModule.h"

// C API Wrapper for BotModule
namespace geblaat {

void CAPI_BotModule::registerBotCommand(const char *command, on_bot_command_f handler) {
    botCommands[command] = handler;
    mBotClient->registerBotCommand(this, command,
                                   [this](std::string command, std::string parameters, std::map<std::string, std::string> message) {
                                       onBotCommand(command, parameters, message);
                                   });
}

void CAPI_BotModule::onBotCommand(std::string command, std::string parameters, std::map<std::string, std::string> recvMessage) {
    if (botCommands.contains(command)) {
        key_value_t *kv = (key_value_t *)calloc(recvMessage.size() + 1, sizeof(key_value_t));
        for (auto &m : recvMessage) {
            kv->key = m.first.c_str();
            kv->value = m.second.c_str();
            kv++;
        }
        botCommands[command](command.c_str(), parameters.c_str(), kv);
        free(kv);
    }
}

} // namespace geblaat

extern "C" {

void register_bot_command(void *module, const char *command, on_bot_command_f handler) {
    geblaat::PluginLoadable *mod = (geblaat::PluginLoadable *)module;
    geblaat::CAPI_BotModule *capi_botmodule = dynamic_cast<geblaat::CAPI_BotModule *>(mod);
    if (capi_botmodule) {
        capi_botmodule->registerBotCommand(command, handler);
    }
}

} // extern "C"
