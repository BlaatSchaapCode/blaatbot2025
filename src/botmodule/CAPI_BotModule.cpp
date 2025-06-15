/*

 Author:	André van Schoubroeck <andre@blaatschaap.be>
 License:	MIT

 SPDX-License-Identifier: MIT

 Copyright (c) 2025 André van Schoubroeck <andre@blaatschaap.be>

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 */

#include "CAPI_BotModule.hpp"
#include "CAPI_BotModule.h"

#include "utils/logger.hpp"

// C API Wrapper for BotModule
namespace geblaat {

CAPI_BotModule::CAPI_BotModule(new_botmodule_instance_f newInstance, del_botmodule_instance_f delInstance) {
    newBotmoduleInstance = newInstance;
    delBotmoduleInstance = delInstance;
    botModuleInstance = newBotmoduleInstance(&botClient);
}

CAPI_BotModule::~CAPI_BotModule() {
    if (delBotmoduleInstance && botModuleInstance)
        delBotmoduleInstance(botModuleInstance);
}

int CAPI_BotModule::setConfig(const nlohmann::json &config) {
    if (!botModuleInstance)
        return -1;
    if (!botModuleInstance->set_config)
        return -1;
    return botModuleInstance->set_config(botModuleInstance, config.dump(4).c_str());
}

nlohmann::json CAPI_BotModule::getConfig(void) {
    nlohmann::json result;
    // TODO: C api for retrieving config from C botmodule
    return result;
}

void CAPI_BotModule::registerBotCommand(const char *command, on_bot_command_callback_f handler) {
    LOG_INFO("Registering bot command '%s' at %p", command, handler);

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
        botCommands[command](botModuleInstance, command.c_str(), parameters.c_str(), message);
        free(message);
    }
}

} // namespace geblaat
