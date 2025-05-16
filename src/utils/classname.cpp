/*
 * classname.cpp
 *
 *  Created on: 26 apr. 2025
 *      Author: andre
 */

// https://gcc.gnu.org/onlinedocs/libstdc++/manual/ext_demangling.html
#include <cxxabi.h>
#include <string>
#include <cstdlib>
std::string demangleClassName(std::string mangledName) {
    int status;
    std::string result = "Unknown";
    char *realname;
    realname = abi::__cxa_demangle(mangledName.c_str(), NULL, NULL, &status);
    if (!status && realname)
    	result = realname;
    if (realname)
    free(realname);
    return result;
}
