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
    botclient_c_api_t botClient = {
        .size = sizeof(botclient_c_api_t),
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
