#pragma once
#include "Logger.hpp"
namespace geblaat {
class ConsoleLogger : public Logger {
  public:
    virtual ~ConsoleLogger() {}
    void Log(LogLevel level, const char *module, const char *filename, const unsigned line, const char *function,
             const char *message) override;
    int setConfig(const nlohmann::json &) override;
};
} // namespace geblaat
