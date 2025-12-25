#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>

#include <dlfcn.h>

#include <toml++/toml.hpp>

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

namespace plugin::artifact {
    Artifact New(void) {
        auto body = std::make_unique<ArtifactBody>();
        body->obj_mode = RTLD_LAZY;
        return std::move(body);
    }

    Artifact New(const ObjectPath path) {
        auto a = New();
        a->obj_path = path;
        return a;
    }

    Artifact WithDescription(const Artifact& source, const Desc desc) {
        auto a = FromOther(source);
        a->description = desc;
        return a;
    }

    Artifact WithDescriptionLong(const Artifact& source, const Desc desc) {
        auto a = FromOther(source);
        a->description_long = desc;
        return a;
    }

    Artifact WithManifest(const Artifact& source, const Path path) {
        auto path_p = std::filesystem::path(path);

        try {
            if (path_p.is_relative())
                path_p = std::filesystem::absolute(path_p);
            if (!std::filesystem::exists(path_p))
                return WithState(source, StateErr::MAN_NOMAN);
        } catch (const std::filesystem::filesystem_error& err) {
            std::cerr << "error reading manifest: " << err.what() << std::endl;
            return WithState(source, StateErr::MAN_MALPARSE);
        }

        auto a = FromOther(source);
        a->manifest = path_p.string();
        return std::move(a);
    }

    Artifact WithName(const Artifact& source, const Name name) {
        auto a = FromOther(source);
        a->name = name;
        return a;
    }

    Artifact WithPath(const Artifact& source, const ObjectPath path) {
        auto a = FromOther(source);
        a->obj_path = path;
        return a;
    }

    Artifact WithState(const Artifact& source, const StateErr state) {
        auto a = FromOther(source);
        a->state = state;
        return a;
    }

    Artifact WithVersion(const Artifact& source, const Version version) {
        auto a = FromOther(source);
        a->version = version;
        return a;
    }

    Artifact FromTomlWithDescription(const Artifact& a, toml::table& tbl) {
        auto desc = tbl["description"].value_or<std::string>("none");
        return WithDescription(a, desc);
    }

    Artifact FromTomlWithDescriptionLong(const Artifact& a, toml::table& tbl) {
        auto desc = tbl["description_long"].value_or<std::string>("none");
        return WithDescriptionLong(a, desc);
    }

    Artifact FromTomlWithName(const Artifact& a, toml::table& tbl) {
        auto name = tbl["name"].value_or<std::string>("unknown");
        return WithName(a, name);
    }

    Artifact FromTomlWithPath(const Artifact& a, toml::table& tbl) {
        auto mpath = std::filesystem::path(a->manifest);
        auto opath = tbl["path"].value_or<Path>("missing");
        if (opath != "missing" && std::filesystem::path(opath).is_relative())
            opath = mpath.parent_path() / opath;
        if (opath != "missing" && !std::filesystem::exists(opath))
            return WithState(a, StateErr::DLL_NOMODULE);
        return WithPath(a, opath);
    }

    Artifact FromTomlWithVersion(const Artifact& a, toml::table& tbl) {
        auto version = tbl["version"].value_or<Version>("0.0.0");
        return WithVersion(a, version);
    }

    Artifact FromTomlPluginHeader(const Artifact& a, toml::table& tbl) {
        auto amod = FromOther(a);
        try {
            if (!tbl["plugin"])
                return WithState(a, StateErr::MAN_NOPLUGIN);
            auto plugin_header = *(tbl["plugin"].as_table());

            amod = FromTomlWithDescription(amod, plugin_header);
            amod = FromTomlWithDescriptionLong(amod, plugin_header);
            amod = FromTomlWithPath(amod, plugin_header);
            amod = FromTomlWithName(amod, plugin_header);
            amod = FromTomlWithVersion(amod, plugin_header);

        } catch (const toml::parse_error& err) {
            std::cerr << "error loading manifest: " << err.what() << std::endl;
            return WithState(a, StateErr::MAN_MALPARSE);
        }

        return amod;
    }

    Artifact FromToml(const Artifact& a) {
        toml::table t;
        try {
            t = toml::parse_file(a->manifest);
        } catch (const toml::parse_error& err) {
            std::cerr << "error loading manifest: " << err.what() << std::endl;
            return WithState(a, StateErr::MAN_MALPARSE);
        }

        return FromTomlPluginHeader(a, t);
    }

    Artifact FromFile(const char *path) {
        auto a = WithManifest(New(), path);
        if (a->state != StateErr::NONE) return std::move(a);
        return FromToml(std::move(a));
    }

    Artifact FromOther(const Artifact& source) {
        auto a = New();
        a->commands         = source->commands;
        a->description      = source->description;
        a->description_long = source->description_long;
        a->manifest         = source->manifest;
        a->name             = source->name;
        a->obj_mode         = source->obj_mode;
        a->obj_path         = source->obj_path;
        a->obj_ref          = source->obj_ref;
        a->state            = source->state;
        a->version          = source->version;
        return a;
    }

    std::ostream& operator<<(std::ostream& os, const Artifact& a) {
        os << "Artifact[";
            os << "name=\"" << a->name << "\", ";
            os << "description=\"" << a->description << "\", ";
            os << "description_long=\"" << a->description_long << "\", ";
            os << "manifest=\"" << a->manifest << "\", ";
            os << "dlpath=\"" << a->obj_path << "\", ";
            os << "dlref=" << a->obj_ref << ", ";
            os << "state=" << a->state << ", ";
            os << "version=\"" << a->version << "\"";
        return os << "]";
    }
}

namespace plugin::dllutil {
    using Artifact = artifact::Artifact;

    State<Artifact> HandleErr(const Artifact& source, StateErr err) {
        source->state = err;
        return artifact::FromOther(source);
    }

    auto HandleErr(const Artifact& source) {
        return [&source](StateErr err){
            return HandleErr(source, err);
        };
    }

    State<void*> LoadHandleFromArtifact(const Artifact& source, const Name name) {
        if (source->state != NONE) return std::unexpected(DLL_NOMODULE);
        void *handle = dlsym(source->obj_ref, name.c_str());
        if (handle == nullptr) return std::unexpected(DLL_NOOBJECT);
        return handle;
    }

    State<void*> LoadModuleFromArtifact(const Artifact& source) {
        if (!std::filesystem::exists(source->obj_path)) return std::unexpected(DLL_NOMODULE);
        void *module = dlopen(source->obj_path.c_str(), source->obj_mode);
        if (module == nullptr) return std::unexpected(DLL_MALFORMAT);
        return module;
    }

    State<Artifact> InstallModule(const Artifact& source) {
        auto a = artifact::FromOther(source);
        return LoadModuleFromArtifact(a)
            .transform_error([&a](StateErr err){
                a->state = err;
                return err;
            })
            .transform([&a](void *module){
                a->obj_ref = module;
                return std::move(a);
            });
    }

    State<Artifact> InstallCommand(const Artifact& source, Name name) {
        auto a = artifact::FromOther(source);
        a->commands[name] = command::LoadFromArtifact(a, name);
        if (a->commands[name].state != NONE) {
            a->state = a->commands[name].state;
            return std::unexpected(a->state);
        }
        return a;

    }

    auto InstallCommand(Name name) {
        return [name](const Artifact& source){
            return InstallCommand(source, name);
        };
    }

    State<Artifact> CallCommand(const Artifact& source, Name name) {
        if (source->commands[name].state == NONE) {
            source->state = source->commands[name].impl(nullptr);
            return artifact::FromOther(source);
        }
        return std::unexpected(source->commands[name].state);
    }

    auto CallCommand(Name name) {
        return [name](const Artifact& source){
            return CallCommand(source, name);
        };
    }

    Artifact Install(const Artifact& source) {
        return InstallModule(source)
            .and_then(InstallCommand("OnInstall"))
            .and_then(InstallCommand("OnRelease"))
            .and_then(CallCommand("OnInstall"))
            .or_else(HandleErr(source))
            .value();
    }

    Artifact Release(const Artifact& source) {
        return CallCommand(source, "OnRelease")
            .or_else(HandleErr(source))
            .value();
    }
}

namespace plugin::command {
    using Artifact = artifact::Artifact;

    State<CommandImpl*> LoadCommandImplFromArtifact(const Artifact& source, const Name name) {
        return dllutil::LoadHandleFromArtifact(source, name + "_callbackImpl")
            .transform_error([](StateErr err) {
                if (err == DLL_NOMODULE) return err;
                return DLL_NOCALLBACK;
            })
            .transform([](void *handle){
                return (CommandImpl*)(handle);
            });
    }

    State<CommandScope*> LoadCommandScopeFromArtifact(const Artifact& source, const Name name) {
        return dllutil::LoadHandleFromArtifact(source, name + "_callbackScope")
            .transform_error([](StateErr err) {
                if (err == DLL_NOMODULE) return err;
                return DLL_NOSCOPE;
            })
            .transform([](void *handle){
                return (CommandScope*)(handle);
            });
    }

    Command LoadFromArtifact(const Artifact& source, Name name) {
        Command cmd = {0};

        auto scope = LoadCommandScopeFromArtifact(source, name);
        if (!scope.has_value()) {
            cmd.state = scope.error();
            return cmd;
        }

        auto impl = LoadCommandImplFromArtifact(source, name);
        if (!impl.has_value()) {
            cmd.state = impl.error();
            return cmd;
        }

        cmd.impl = *impl.value();
        cmd.scope = *scope.value();
        return cmd;
    }
}

namespace plugin::plugin {
    Plugin::Plugin(const Context _) {}
}
