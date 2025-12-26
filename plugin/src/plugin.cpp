#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>

#include <dlfcn.h>

#include "plugin.hpp"

namespace plugin {
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

    std::ostream& operator<<(std::ostream& os, const StateErr& state) {
        return os << "<"
            << (std::size_t)state << ":"
            << StateGroup(state) << ":\""
            << StateMessage(state)
            << "\">";
    }
}

namespace plugin::plugin {
    Plugin::Plugin(const Context _) {}
}
