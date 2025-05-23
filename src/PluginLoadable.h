#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef struct {
    const char name[32];
    const char description[128];
    struct {
        uint32_t version;
    } abi;
} pluginloadable_t;

#ifdef __cplusplus
}
#endif
