#pragma once

#include <string>
#include <map>

#include "BotModule.hpp"



namespace geblaat {

class TestBotModule : public BotModule {
public:
	virtual ~TestBotModule();
	int setConfig(nlohmann::json) override;
private:
	void onTest(std::string command, std::string parameters, std::map<std::string, std::string> message);
};

}
