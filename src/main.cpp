/*

 File: 		main.cpp
 Author:	André van Schoubroeck <andre@blaatschaap.be>
 License:	MIT


 SPDX-License-Identifier: MIT

 Copyright (c) 2025 André van Schoubroeck <andre@blaatschaap.be>

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 */

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

// Configures the console to use UTF8 on WIN32
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

#include "HTTP.hpp"

void test() {
    auto httplib_instance = mPluginLoader.newInstance("httplib", "protocol");

    auto pr = dynamic_cast<geblaat::Protocol *>(httplib_instance);

    geblaat::HTTP *httplib = dynamic_cast<geblaat::HTTP *>(httplib_instance);
    if (httplib) {
        httplib->get("https://www.blaatschaap.be");
        httplib->get("https://www.blaatschaap.be/test");
    }

    auto simplehttp_instance = mPluginLoader.newInstance("simplehttp", "protocol");
    geblaat::HTTP *simplehttp = dynamic_cast<geblaat::HTTP *>(simplehttp_instance);
    if (simplehttp) {
        simplehttp->get("https://www.blaatschaap.be");
    }
}

int main(int argc, char *argv[]) {
    int result;
    utils::Version version;

    test();

    geblaat::Client *client = nullptr;
    //
    //    result = parse_options(argc, argv);
    //    if (result)
    //        return result;
    //
    //    if (mConfigdata.size()) {
    //        LOG_INFO("Config file loaded");
    //        try {
    //            auto jsonClient = mConfigdata["client"];
    //            client = dynamic_cast<geblaat::Client *>(mPluginLoader.newInstance(jsonClient["name"], "client"));
    //            if (client) {
    //                client->setConfig(jsonClient["config"]);
    //            } else {
    //                throw std::runtime_error("Unable to get a client");
    //            }
    //        } catch (nlohmann::json::exception &ex) {
    //            LOG_ERROR("JSON exception: %s", ex.what());
    //            return -1;
    //        } catch (std::exception &ex) {
    //            LOG_ERROR("Unknown exception: %s", ex.what());
    //            return -1;
    //        } catch (...) {
    //            LOG_ERROR("Unknown exception (not derived from std::exception)");
    //            return -1;
    //        }
    //
    //    } else {
    //        LOG_INFO("Config file missing");
    //    }

    LOG_INFO("press ENTER key to quit");
    std::cin.get();

    if (client)
        delete client;

    return 0;
}
