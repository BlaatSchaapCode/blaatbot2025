/*
 * connection.cpp
 *
 *  Created on: 7 mrt. 2025
 *      Author: andre
 */

#include "connection.hpp"
#include <iostream>
#include <string>
#include <vector>

namespace network {

Connection::~Connection() {}

void Connection::process(std::vector<char> data) {
    // Is it required for the IRC server to send one line per TCP packet?
    if (mLineMode) {
        mBuffer.insert(mBuffer.end(), data.begin(), data.end());

        while (true) {
            std::string eolMarker = "\r\n";
            auto end = std::search(mBuffer.begin(), mBuffer.end(), eolMarker.begin(), eolMarker.end());
            if (end != mBuffer.end()) {
                std::string line = std::string(mBuffer.begin(), end);
                mBuffer.erase(mBuffer.begin(), end + 2);
                std::cout << "lineMode " << line << "\n";
            } else
                break;
        }
    } else {
        std::string line = std::string(data.begin(), data.end() - 2);
        std::cout << "rawMode  " << line << "\n";
    }
}
} // namespace network
