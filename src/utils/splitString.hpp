/*
 * splitString.hpp
 *
 *  Created on: 8 mrt. 2025
 *      Author: andre
 */

#ifndef UTILS_SPLITSTRING_HPP_
#define UTILS_SPLITSTRING_HPP_
#include <map>
#include <string>
#include <vector>

std::vector<std::string> splitString(const std::string &str, const std::string &delimiter = " ",
                                     const unsigned int max_elements = 0);

#endif /* UTILS_SPLITSTRING_HPP_ */
