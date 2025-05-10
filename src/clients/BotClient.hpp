#pragma once

// C++ Library Includes
#include <map>
#include <memory>
#include <string>
#include <functional>

// Third Party libraries
#include <nlohmann/json.hpp>

// Project includes
#include "Protocol.hpp"

#include "Client.hpp"

namespace geblaat {

class  BotClient : public Client {
  public:
	BotClient();
    virtual ~BotClient();
    virtual int setConfig(nlohmann::json) override;

    virtual void onMessage(std::map<std::string, std::string> message) override;

    using OnCommand = std::function<void(std::string command, std::string parameters, std::map<std::string, std::string> message)>;
  protected:
    Protocol *mProtocol = nullptr;

  private:
    std::map < std::string, std::map < std::string, OnCommand >	> mCommands;
};

} // namespace geblaat
