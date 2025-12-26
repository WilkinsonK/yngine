#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>

#include <dlfcn.h>

#include "plugin.hpp"
#include "plugin_internal.hpp"

namespace __plugin_root {
    std::ostream& operator<<(std::ostream& os, const StateErr& state) {
        return os << "<"
            << (std::size_t)state << ":"
            << StateGroup(state) << ":\""
            << StateMessage(state)
            << "\">";
    }

    Context Install(const Context& ctx) {
        return dllutil::InstallModule(ctx)
            .and_then(dllutil::InstallCommand("OnInstall"))
            .and_then(dllutil::InstallCommand("OnRelease"))
            .and_then(dllutil::CallCommand("OnInstall"))
            .or_else(dllutil::HandleErr(ctx))
            .value();
    }

    Context Release(const Context& ctx) {
        return dllutil::CallCommand(ctx, "OnRelease")
            .or_else(dllutil::HandleErr(ctx))
            .value();
    }

    const std::string StateGroup(const StateErr& state) {
        std::stringstream ss;

        switch (state / 0x1000) {
            case 0:
                ss << "NIL";
                break;
            case 0x1:
                ss << "DLL";
                break;
            case 0x2:
                ss << "MAN";
                break;
            default:
                ss << "USR";
                break;
        }

        return ss.str();
    }

    const std::string StateMessage(const StateErr& state) {
        std::stringstream ss;

        switch (state) {
            case NONE:
                ss << "no error";
                break;
            case DLL_MALFORMAT:
                ss << "bad dll format";
                break;
            case DLL_NOCALLBACK:
                ss << "missing callback";
                break;
            case DLL_NODROP:
                ss << "missing destroyer";
                break;
            case DLL_NOINIT:
                ss << "missing initializer";
                break;
            case DLL_NOINSTALL:
                ss << "artifact not installed";
                break;
            case DLL_NOMODULE:
                ss << "missing module";
                break;
            case DLL_NOOBJECT:
                ss << "missing module object";
                break;
            case DLL_NORELEASE:
                ss << "artifact not released";
                break;
            case DLL_NOSCOPE:
                ss << "missing scope of module object";
                break;
            case MAN_NOMAN:
                ss << "missing manifest";
                break;
            case MAN_MALPARSE:
                ss << "failure to parse manifest";
                break;
            case MAN_NOPLUGIN:
                ss << "missing plugin header";
                break;
        }

        return ss.str();
    }
}

__ns_cplusplus_head(__plugin_root)
    StateErr CallCommand(const Context *ctx, const char *name) {
        return dllutil::CallCommand(*ctx, name).error_or(NONE);
    }

    const char *GetName(const Context *ctx) {
        return ctx->artifact->name.c_str();
    }

    const char *GetDescription(const Context *ctx) {
        return ctx->artifact->description.c_str();
    }

    const char *GetDescriptionLong(const Context *ctx) {
        return ctx->artifact->description_long.c_str();
    }

    StateErr RegisterCommand(const Context *ctx, const char *name, const char *impl_name) {
        return dllutil::InstallCommand(*ctx, name, impl_name).error_or(NONE);
    }
__ns_cplusplus_tail

namespace __plugin_root::plugin {
    Plugin::Plugin(const Context _) {}
}
