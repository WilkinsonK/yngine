#include <stdio.h>

#include "plugin_module.h"

plugin_module(
    oninstall(Init) {
        printf("Hello, World!\n");
        return NONE;
    }

    onrelease(Drop) {
        printf("Goodbye, World!\n");
        return NONE;
    }
)
