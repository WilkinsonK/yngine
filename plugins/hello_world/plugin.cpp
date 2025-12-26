#include <stdio.h>

#include "plugin_module.h"

plugin_module(
    callback(PrintHello, SCOPE_SHARED) {
        printf("Hello, World!\n");
        return NONE;
    }

    oninstall(Init) {
        int reg;
        reg = RegisterCommand(context, "hello_world.PrintHello", "PrintHello");
        if (reg != 0)
            return (StateErr)reg;
        return (StateErr)reg;
    }

    onrelease(Drop) {
        printf("Goodbye, World!\n");
        return NONE;
    }
)
