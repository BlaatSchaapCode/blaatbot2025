#pragma once

#include "CAPI_BotModule.h"
#include "BotModule.hpp"
#include <map>
#include <string>
namespace geblaat {



class CAPI_BotModule : public BotModule {
public:
	int setConfig(nlohmann::json) override;

	void registerBotCommand(const char *command, on_bot_command_f handler);
private:
	std::map<std::string, on_bot_command_f> botCommands;
	void onBotCommand(std::string command, std::string parameters, std::map<std::string, std::string> recvMessage) ;
};
} // namespace geblaat
