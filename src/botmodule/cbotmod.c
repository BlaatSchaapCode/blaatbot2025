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
#include "cbotmod.h"
#include "PluginLoadable.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int set_config(const void *, const char *);

botmodule_c_api_t *new_botmodule_instance(botclient_c_api_t *client) {
    if (!client)
        return NULL;
    botmodule_c_api_t *module = malloc(sizeof(botmodule_c_api_t));
    if (!module)
        return NULL;
    memset(module, 0, sizeof(botmodule_c_api_t));
    module->size = sizeof(botmodule_c_api_t);
    module->botclient = client;
    module->set_config = set_config;
    return module;
}
void del_botmodule_instance(botmodule_c_api_t *module) { free(module); }

static const char *get_value(const key_value_t *kv, const char *key) {
    while (kv->key) {
        if (!strcmp(key, kv->key))
            return kv->value;
        kv++;
    }
    return NULL;
}
int on_bot_command(const void *m, const char *command, const char *parameters, const key_value_t *message) {
    if (!m)
        return -1;
    const botmodule_c_api_t *module = m;
    if (!module->botclient)
        return -1;

    printf("command    %s\n", command);
    printf("parameters %s\n", parameters);
    const key_value_t *kv = message;
    while (kv->key) {
        printf("       key %s\n", message->key);
        printf("     value %s\n", message->value);
        kv++;
    }

    const char *target_type = get_value(message, "target/type");
    const char *target = get_value(message, "target");
    const char *sender = get_value(message, "sender");

    const char *response_target;
    if (target_type && !strcmp(target_type, "channel")) {
        response_target = target;
    } else {
        response_target = sender;
    }
    if (response_target) {
        key_value_t response[] = {{"text/plain", "Hello from C"}, {"target", response_target}, {"type", "message"}, {NULL, NULL}};
        module->botclient->send_message(module->botclient, response);
    }

    return 0;
}

static int set_config(const void *m, const char *config_json) {
    if (!m)
        return -1;
    const botmodule_c_api_t *module = m;
    if (!module->botclient)
        return -1;

    if (!module->botclient->register_bot_command)
        return -1;
    return module->botclient->register_bot_command(module->botclient, "see", on_bot_command);
}

pluginloadable_t plugin_info = {
    .name = "C bot module",
    .description = "Example bot module using C API",
    .abi = {.abi = pluginloadable_abi_c, .version = 1},
};
