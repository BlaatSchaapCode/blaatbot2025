#pragma once

#include <vector>
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
	void onRandomQuote(std::string command, std::string parameters, std::map<std::string, std::string> message);
	void onLoadQuotes(std::string command, std::string parameters, std::map<std::string, std::string> me);

	std::string quoteFileName;
	std::vector<std::string> mQuotes;
	std::string getRandomQuote(void);
	void loadQuotes(void);



};

}
