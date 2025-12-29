#include <stdio.h>

#include "plugin_module.h"

plugin_module(
    command(PrintHello, SCOPE_SHARED) {
        printf("Hello, World! (From %s)\n", plugin_get_name());
        return NONE;
    }

    command(PrintFarewell, SCOPE_SHARED) {
        printf("Farewell, World! (From %s)\n", plugin_get_name());
        return NONE;
    }

    oninstall(Init) {
        plugin_register_command("hello_world.PrintHello", "PrintHello");
        plugin_register_command("hello_world.PrintFarewell", "PrintFarewell");
        plugin_call_command("hello_world.PrintHello");
        return NONE;
    }

    onrelease(Drop) {
        plugin_call_command("hello_world.PrintFarewell");
        return NONE;
    }
)
