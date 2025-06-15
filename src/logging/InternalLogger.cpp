#include "InternalLogger.hpp"
#include "PluginLoader.hpp"
namespace geblaat {

InternalLogger::InternalLogger(PluginLoader *pl) {
    // Quick and dirty testing, can we load a logger and use it?
    // TODO: Load them through settings
    pluginLoader = pl;
}

void InternalLogger::Log(Logger::LogLevel level, const char *module, const char *filename, const unsigned line,
                         const char *function, const char *message) {
    for (auto &logger : loggers) {
        logger->Log(level, module, filename, line, function, message);
    }
}
int InternalLogger::setConfig(const nlohmann::json &) {
    // TODO
    return 0;
}

void InternalLogger::test(void) {
    auto logger_plugin = pluginLoader->newInstance("console", "logger");
    auto logger = dynamic_cast<Logger *>(logger_plugin);
    if (logger)
        loggers.emplace_back(logger);

    Log(Logger::LogLevel::Info, MODULE, __FILENAME__, __LINE__, __FUNCTION__, "hello world!");
}

} // namespace geblaat
