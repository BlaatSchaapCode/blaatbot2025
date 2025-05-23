#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef enum {
    pluginloadable_abi_undefined = 0,
    pluginloadable_abi_cpp = 1,
    pluginloadable_abi_c = 2,
} pluginloadable_abi_t;

typedef struct {
    const char name[32];
    const char description[128];
    struct {
        pluginloadable_abi_t abi : 32;
        uint32_t version;
    } abi;
} pluginloadable_t;

#ifdef __cplusplus
}
#endif
