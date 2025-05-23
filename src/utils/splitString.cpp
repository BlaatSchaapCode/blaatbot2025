/*
 * splitString.cpp
 *
 *  Created on: 8 mrt. 2025
 *      Author: andre
 */

#include "splitString.hpp"

#include "logger.hpp"

// https://stackoverflow.com/questions/5607589/right-way-to-split-an-stdstring-into-a-vectorstring
std::vector<std::string> splitString(const std::string &str, const std::string &delimiter, const unsigned int max_elements) {
    std::vector<std::string> tokens;
    std::string::size_type start_index = 0;
    while (true) {
        std::string::size_type next_index = str.find(delimiter, start_index);
        if (next_index == std::string::npos) {
            tokens.push_back(str.substr(start_index));
            break;
        } else {
            tokens.push_back(str.substr(start_index, next_index - start_index));
            start_index = next_index + delimiter.length();
        }
        if (max_elements > 0 && tokens.size() == max_elements - 1) {
            tokens.push_back(str.substr(start_index));
            break;
        }
    }
    return tokens;
}
