#include "ConsoleLogger.hpp"

#include <iostream>
#include <string>

#include <time.hpp>

namespace geblaat {

void ConsoleLogger::Log(LogLevel level, const char *, const char *filename, const unsigned line, const char *function,
                        const char *message) {
    printf("%s [%8s]%16s:%-4d %s\n", getTimeString().c_str(), getLabel(level), filename, line, message);
}

int ConsoleLogger::setConfig(const nlohmann::json &) { return 0; }

} // namespace geblaat

#ifdef DYNAMIC_LIBRARY
extern "C" {
geblaat::ConsoleLogger *newInstance(void) { return new geblaat::ConsoleLogger(); }
void delInstance(geblaat::ConsoleLogger *inst) { delete inst; }
pluginloadable_t plugin_info = {
    .name = "ConsoleLogging",
    .description = "Console Logging",
    .abi = {.abi = pluginloadable_abi_cpp, .version = 0},
};
}
#endif
