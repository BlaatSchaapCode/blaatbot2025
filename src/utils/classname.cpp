/*
 * classname.cpp
 *
 *  Created on: 26 apr. 2025
 *      Author: andre
 */

// https://gcc.gnu.org/onlinedocs/libstdc++/manual/ext_demangling.html
#include <cxxabi.h>
#include <string>
std::string demangleClassName(std::string mangledName) {
    int status;
    char *realname;
    std::string result;
    realname = abi::__cxa_demangle(mangledName.c_str(), NULL, NULL, &status);
    result = realname;
    std::free(realname);
    return result;
}
