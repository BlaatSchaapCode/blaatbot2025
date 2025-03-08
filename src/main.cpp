// SPDX-License-Identifier: MIT

// C++ Library Includes
#include <cstdint>
#include <iostream>
#include <string>

// Library Includes
#include <cxxopts.hpp>

#include "network/cTcpConnection.hpp"
#include "network/cTlsConnection.hpp"
// Game includes
#include "network/network.hpp"
#include "protocol/cIRC.hpp"
#include "utils/logger.hpp"
#include "utils/version.hpp"

int parse_options(int argc, char *argv[]) {
    try {
        cxxopts::Options options(*argv, "BlaatBot2025");

        options.add_options()("h,help", "Display help");

        auto result = options.parse(argc, argv);

        if (result["help"].as<bool>()) {
            std::cout << options.help();
            return 0;
        }

        return 0;

        // New version of cxxopts??? or why did this stop working?
        //    } catch (const cxxopts::OptionException &e) {
        //        // Error parsing the command line
        //        std::cout << "Failed to parse command line options" << std::endl;
        //        std::cout << e.what() << std::endl;
        //        return -1;
        //    }

    } catch (...) {
        std::cout << "Failed to parse command line options" << std::endl;
        return -1;
    }
}

int main(int argc, char *argv[]) {
    int result;
    utils::Version version;

    network::init();

    // auto irc = std::make_shared<protocol::cIRC>;
    auto irc = std::make_shared<protocol::cIRC>("bb25");
    auto tls = std::make_shared<network::cTlsConnection>(irc);
    irc->setConnection(tls);

    // irc->setConnection(test);
    // test.connect("irc.blaatschaap.be", 6667);
    tls->connect("irc.blaatschaap.be", 6697);

    result = parse_options(argc, argv);
    if (result)
        return result;

    LOG_INFO("press ENTER key to quit");
    std::cin.get();

    network::deinit();

    return 0;
}
