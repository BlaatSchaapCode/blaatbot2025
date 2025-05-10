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

typedef void (*on_bot_command_f)(const char *command, const char *parameters, const key_value_t *message);
void register_bot_command(void* module, const char *command, on_bot_command_f handler);


#ifdef __cplusplus
}
#endif
