#include "CAPI_BotModule.hpp"
#include "CAPI_BotModule.h"

// C API Wrapper for BotModule
namespace geblaat {

int CAPI_BotModule::setConfig(nlohmann::json config) {
    if (!botModule)
        return -1;
    if (!botModule->set_config)
        return -1;
    return botModule->set_config(config.dump(4).c_str());
}

CAPI_BotModule::CAPI_BotModule(set_botclient_f bc, get_botmodule_f bm) {
    bc(&botClient);
    botModule = bm();
}

void CAPI_BotModule::registerBotCommand(const char *command, on_bot_command_callback_f handler) {
    botCommands[command] = handler;
    mBotClient->registerBotCommand(this, command,
                                   [this](std::string command, std::string parameters, std::map<std::string, std::string> message) {
                                       onBotCommand(command, parameters, message);
                                   });
}

void CAPI_BotModule::onBotCommand(std::string command, std::string parameters, std::map<std::string, std::string> recvMessage) {
    if (botCommands.contains(command)) {
        key_value_t *message = (key_value_t *)calloc(recvMessage.size() + 1, sizeof(key_value_t));
        key_value_t *kv = message;
        for (auto &m : recvMessage) {
            kv->key = m.first.c_str();
            kv->value = m.second.c_str();
            kv++;
        }
        botCommands[command](command.c_str(), parameters.c_str(), message);
        free(message);
    }
}

} // namespace geblaat
