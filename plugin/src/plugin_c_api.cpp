#include "plugin_c_api.h"

extern "C" {
    StateErr plugin_call_command(const Context *ctx, const Name name) {
        return plugin::CallCommand(ctx, name);
    }

    const char *plugin_get_description(const Context *ctx) {
        return plugin::GetDescription(ctx);
    }

    const char *plugin_get_description_long(const Context *ctx) {
        return plugin::GetDescriptionLong(ctx);
    }

    const char *plugin_get_name(const Context *ctx) {
        return plugin::GetName(ctx);
    }

    StateErr plugin_register_command(const Context *ctx, const Name name, const Name impl_name) {
        return plugin::RegisterCommand(ctx, name, impl_name);
    }
}
