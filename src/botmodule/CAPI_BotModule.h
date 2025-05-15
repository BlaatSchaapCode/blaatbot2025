#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// 	void onTest(std::string command, std::string parameters, std::map<std::string, std::string> message);
//  void BotClient::registerBotCommand(BotModule *mod, std::string command, OnCommand cmd)

typedef struct {
    const char *key;
    const char *value;
} key_value_t;

typedef int (*on_bot_command_callback_f)(const char *command, const char *parameters, const key_value_t *message);
typedef int (*register_bot_command_f)(const void *client, const char *command, on_bot_command_callback_f handler);
typedef int (*send_message_f)(const void *client, const key_value_t *message);

typedef int (*set_config_f)(const char *config_json);

typedef struct {
    set_config_f set_config;
} botmodule_c_api_t;

typedef struct {
    const void *client;
    register_bot_command_f register_bot_command;
    send_message_f send_message;
} botclient_c_api_t;

typedef void (*set_botclient_f)(botclient_c_api_t *botclient);
typedef botmodule_c_api_t *(*get_botmodule_f)(void);

#ifdef __cplusplus
}
#endif
