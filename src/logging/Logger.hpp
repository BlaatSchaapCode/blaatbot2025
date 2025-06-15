#pragma once
#include "PluginLoader.hpp"

namespace geblaat {
class Logger : public PluginLoadable {
  public:
    enum class LogLevel {
        Error,
        Warning,
        Info,
        Debug,
    };
    static const char *getLabel(LogLevel level) {
        switch (level) {
        case LogLevel::Warning:
            return "WARNING";
        case LogLevel::Error:
            return "ERROR";
        case LogLevel::Debug:
            return "DEBUG";
        default:
            return "LOG";
        }
    }
    virtual void Log(LogLevel level, const char *module, const char *filename, const unsigned line, const char *function,
                     const char *message) = 0;
};

} // namespace geblaat
