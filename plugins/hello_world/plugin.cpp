#include <stdio.h>

#include "plugin_module.h"

plugin_module(
    callback(PrintHello, SCOPE_SHARED) {
        printf("Hello, World!\n");
        return NONE;
    }

    callback(PrintFarewell, SCOPE_SHARED) {
        printf("Farewell, World!\n");
        return NONE;
    }

    oninstall(Init) {
        StateErr reg;
        reg = RegisterCommand(context, "hello_world.PrintHello", "PrintHello");
        if (reg != 0) return reg;

        reg = RegisterCommand(context, "hello_world.PrintFarewell", "PrintFarewell");
        if (reg != 0) return reg;

        reg = CallCommand(context, "hello_world.PrintHello");
        if (reg != 0) return reg;
        return reg;
    }

    onrelease(Drop) {
        StateErr reg = CallCommand(context, "hello_world.PrintFarewell");
        if (reg != 0) return reg;
        return NONE;
    }
)
