// SPDX-License-Identifier: MIT

// C++ Library Includes
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

// Library Includes
#include <cxxopts.hpp>
#include <nlohmann/json.hpp>

#include "clients/Client.hpp"
#include "protocol/IRC.hpp"
#include "utils/logger.hpp"
#include "utils/version.hpp"

#include "PluginLoader.hpp"
static geblaat::PluginLoader mPluginLoader;

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>

[[gnu::constructor]] void windows_init_utf8_console() {
    auto a = SetConsoleOutputCP(CP_UTF8);
    auto b = SetConsoleCP(CP_UTF8);
    (void)a;
    (void)b;
    // Note, these APIs have non-zero value for success
    LOG_DEBUG("WIN32 setting output to UTF8 status %d %d", a, b);

    // Does not appear to output currectly when running under wine
    // Works in a real Windows 10
    // Note: requires Windows 10 version 1903 or later.
    LOG_DEBUG("test: äåéëþüúíóö«»¬");
}

#endif

static nlohmann::json mConfigdata;

int parse_options(int argc, char *argv[]) {
    try {
        cxxopts::Options options(*argv, "BlaatBot2025");

        options.add_options()("c,config", "Configuration file", cxxopts::value<std::string>());
        options.add_options()("h,help", "Display help");

        auto result = options.parse(argc, argv);

        if (result.count("help")) {
            std::cout << options.help();
            return 0;
        }

        if (result.count("config")) {
            std::string configfile = result["config"].as<std::string>();
            std::ifstream f(configfile);
            nlohmann::json configdata = nlohmann::json::parse(f);
            f.close();
            mConfigdata = configdata;
            return 0;
        }

        return 0;

    } catch (const cxxopts::exceptions::exception &ex) {
        // Error parsing the command line
        std::cout << "Failed to parse command line options" << std::endl;
        std::cout << ex.what() << std::endl;
        return -1;
    } catch (std::ifstream::failure &ex) {
        std::cerr << "Exception opening/reading/closing config file\n";
        std::cout << ex.what() << std::endl;
        return -1;
    } catch (nlohmann::json::exception &ex) {
        std::cerr << "Exception parsing config file\n";
        std::cout << ex.what() << std::endl;
        return -1;
    } catch (std::exception &ex) {
        std::cerr << "Unknown exception (derived from std::exception)\n";
        std::cout << ex.what() << std::endl;
        return -1;
    } catch (...) {
        std::cerr << "Unknown exception (not derived from std::exception)\n";
        return -1;
    }
}

#include "osVersion.hpp"

int main(int argc, char *argv[]) {
    int result;
    utils::Version version;

    geblaat::Client *client = nullptr;

    result = parse_options(argc, argv);
    if (result)
        return result;

    if (mConfigdata.size()) {
        LOG_INFO("Config file loaded");
        try {
            auto jsonClient = mConfigdata["client"];
            client = dynamic_cast<geblaat::Client *>(mPluginLoader.newInstance(jsonClient["type"], "client"));
            if (client) {
                client->setConfig(jsonClient["config"]);
            } else {
                throw std::runtime_error("Unable to get a client");
            }
        } catch (nlohmann::json::exception &ex) {
            LOG_ERROR("JSON exception: %s", ex.what());
            return -1;
        } catch (std::exception &ex) {
            LOG_ERROR("Unknown exception: %s", ex.what());
            return -1;
        } catch (...) {
            LOG_ERROR("Unknown exception (not derived from std::exception)");
            return -1;
        }

    } else {
        LOG_INFO("Config file missing");
    }

    LOG_INFO("press ENTER key to quit");
    std::cin.get();

    if (client)
        delete client;

    return 0;
}
