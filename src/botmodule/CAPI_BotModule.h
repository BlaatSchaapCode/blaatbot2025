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

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
    const char *key;
    const char *value;
} key_value_t;

typedef int (*on_bot_command_callback_f)(const char *command, const char *parameters, const key_value_t *message);
typedef int (*register_bot_command_f)(const void *client, const char *command, on_bot_command_callback_f handler);
typedef int (*send_message_f)(const void *client, const key_value_t *message);

typedef int (*set_config_f)(const char *config_json);

typedef struct {
    uint32_t size;
    set_config_f set_config;
} botmodule_c_api_t;

typedef struct {
    uint32_t size;
    const void *client;
    register_bot_command_f register_bot_command;
    send_message_f send_message;
} botclient_c_api_t;

typedef void (*set_botclient_f)(botclient_c_api_t *botclient);
typedef botmodule_c_api_t *(*get_botmodule_f)(void);

#ifdef __cplusplus
}
#endif
