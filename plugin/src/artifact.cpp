#include <dlfcn.h>

#include <toml++/toml.hpp>

#include "plugin.hpp"

namespace __plugin_root::artifact {

    Artifact New(void) {
        auto body = std::make_unique<ArtifactBody>();
        body->obj_mode = RTLD_LAZY;
        return std::move(body);
    }

    Artifact New(const ObjectEntry path) {
        auto a = New();
        a->obj_entry = path;
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

    Artifact WithPath(const Artifact& source, const ObjectEntry path) {
        auto a = FromOther(source);
        a->obj_entry = path;
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
        auto opath = tbl["entry"].value_or<Path>("missing");
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
        a->obj_entry        = source->obj_entry;
        a->obj_mode         = source->obj_mode;
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
            os << "dlpath=\"" << a->obj_entry << "\", ";
            os << "dlref=" << a->obj_ref << ", ";
            os << "state=" << a->state << ", ";
            os << "version=\"" << a->version << "\"";
        return os << "]";
    }
}