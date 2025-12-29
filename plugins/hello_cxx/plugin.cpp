#include <iostream>

#include "plugin_module.h"

plugin_module(
    command(PrintHello, SCOPE_SHARED) {
        std::cout << "Hello, World! (From " << plugin_get_name() << ")" << std::endl;
        return NONE;
    }

    command(PrintFarewell, SCOPE_SHARED) {
        std::cout << "Farewell, World! (From " << plugin_get_name() << ")" << std::endl;
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
