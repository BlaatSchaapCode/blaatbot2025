#pragma once

#pragma once

#include <string>
#include <vector>

#include "Logger.hpp"
#include "PluginLoadable.hpp"
#include "PluginLoader.hpp"

namespace geblaat {
class InternalLogger : public Logger {
  public:
    InternalLogger(PluginLoader *pl);
    virtual ~InternalLogger() {};

    void Log(Logger::LogLevel level, const char *module, const char *filename, const unsigned line, const char *function,
             const char *message);

    int setConfig(const nlohmann::json &);

    void test(void);

  private:
    geblaat::PluginLoader *pluginLoader;
    std::vector<Logger *> loggers;
};

} // namespace geblaat
