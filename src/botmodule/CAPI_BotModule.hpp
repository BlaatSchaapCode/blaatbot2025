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

#pragma once

#include "BotModule.hpp"
#include "CAPI_BotModule.h"
#include <string>
#include <unordered_map>
namespace geblaat {

class CAPI_BotModule : public BotModule {
  public:
    int setConfig(const nlohmann::json &) override;
    nlohmann::json getConfig(void) override;
    CAPI_BotModule(new_botmodule_instance_f, del_botmodule_instance_f);
    virtual ~CAPI_BotModule() {}
    void registerBotCommand(const char *command, on_bot_command_callback_f handler);

  private:
    std::unordered_map<std::string, on_bot_command_callback_f> botCommands;
    void onBotCommand(std::string command, std::string parameters, std::map<std::string, std::string> recvMessage);

    new_botmodule_instance_f newBotmoduleInstance = nullptr;
    del_botmodule_instance_f delBotmoduleInstance = nullptr;
    botmodule_c_api_t *botModuleInstance = nullptr;

    botclient_c_api_t botClient = {
        .size = sizeof(botclient_c_api_t),
        .client = this,
        .register_bot_command =
            [](const void *cc, const char *command, on_bot_command_callback_f handler) {
                botclient_c_api_t *c = (botclient_c_api_t *)(cc);
                CAPI_BotModule *self = (CAPI_BotModule *)(c->client);
                self->registerBotCommand(command, handler);
                return 0;
            },
        .send_message =
            [](const void *cc, const key_value_t *kv) {
                botclient_c_api_t *c = (botclient_c_api_t *)(cc);
                CAPI_BotModule *self = (CAPI_BotModule *)(c->client);
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
