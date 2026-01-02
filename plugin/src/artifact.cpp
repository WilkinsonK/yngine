#include <sstream>
#include <vector>

#include <dlfcn.h>

#include <toml++/toml.hpp>

#include "plugin.hpp"

namespace __plugin_root::artifact {
    typedef std::function<Artifact(const Artifact&, toml::table&)> HeaderExtractor;
    typedef std::vector<HeaderExtractor> HeaderExtractors;

    bool ArtifactBody::IsInstalled(void) {
        return state.code == NONE && obj_ref != nullptr;
    }

    void ArtifactBody::SetState(const StateInfo info) {
        this->state = info;
    }

    const Name GetName(const Artifact& source) {
        return source->name;
    }

    const Desc GetDescription(const Artifact& source) {
        return source->description;
    }

    const Desc GetDescriptionLong(const Artifact& source) {
        return source->description_long;
    }

    const ObjectEntry GetEntry(const Artifact& source) {
        return source->obj_entry;
    }

    const ObjectMode GetMode(const Artifact& source) {
        return source->obj_mode;
    }

    const ObjectType GetType(const Artifact& source) {
        return source->obj_type;
    }

    Artifact New(void) {
        auto body = std::make_unique<ArtifactBody>();
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

    Artifact WithEntry(const Artifact& source, const ObjectEntry entry) {
        auto a = FromOther(source);
        a->obj_entry = entry;
        return a;
    }

    Artifact WithManifest(const Artifact& source, const Path path) {
        auto path_p = std::filesystem::path(path);

        try {
            if (path_p.is_relative())
                path_p = std::filesystem::absolute(path_p);
            if (!std::filesystem::exists(path_p))
                return WithState(source, StateErr::MAN_NOMAN, path_p);
        } catch (const std::filesystem::filesystem_error& err) {
            return WithState(source, StateErr::MAN_MALPARSE, err.what());
        }

        auto a = FromOther(source);
        a->manifest = path_p.string();
        return std::move(a);
    }

    Artifact WithMode(const Artifact& source, const ObjectMode mode) {
        auto a = FromOther(source);
        a->obj_mode = mode;
        return a;
    }

    Artifact WithName(const Artifact& source, const Name name) {
        auto a = FromOther(source);
        a->name = name;
        return a;
    }

    Artifact WithState(const Artifact& source, const StateErr state) {
        return WithState(source, state, "");
    }

    Artifact WithState(const Artifact& source, const StateErr state, const StateErrMessage detail) {
        auto a = FromOther(source);
        a->SetState(state & detail);
        return a;
    }

    Artifact WithType(const Artifact& source, const ObjectType type) {
        auto a = FromOther(source);
        a->obj_type = type;
        return a;
    }

    Artifact WithVersion(const Artifact& source, const Version version) {
        auto a = FromOther(source);
        a->version = version;
        return a;
    }

    // This operation is only needed as a macro as it is
    // repeatable, but not worth adding on to the call
    // stack. Nor is it a viable option to make this an
    // inline function as the overhead isn't worth it.
    #define __tbl_extract_str(T, NAME, DEFAULT) T[NAME].value_or<std::string>(DEFAULT)
    #define __fld_desc   "description"
    #define __fld_descl  "description_long"
    #define __fld_entry  "entry"
    #define __fld_mode   "mode"
    #define __fld_name   "name"
    #define __fld_type   "type"
    #define __fld_vers   "version"
    #define __hdr_plugin "plugin"
    #define __hdr_module "module"
    #define __val_anon   "anonymous"
    #define __val_dyn    "dynamic"
    #define __val_miss   "missing"
    #define __val_none   "none"

    #define __is_missing(str) str == __val_miss

    Artifact FromTomlWithDescription(const Artifact& a, toml::table& tbl) {
        return WithDescription(a, __tbl_extract_str(tbl, __fld_desc, __val_none));
    }

    Artifact FromTomlWithDescriptionLong(const Artifact& a, toml::table& tbl) {
        return WithDescriptionLong(a, __tbl_extract_str(tbl, __fld_descl, __val_none));
    }

    Artifact FromTomlWithEntry(const Artifact& a, toml::table& tbl) {
        auto mpath = std::filesystem::path(a->manifest);
        auto opath = __tbl_extract_str(tbl, __fld_entry, __val_miss);
        // If `opath` is not missing, and is relative,
        // assume is relative to the plugin project.
        if (!(__is_missing(opath)) && std::filesystem::path(opath).is_relative())
            opath = mpath.parent_path() / opath;
        // If `opath` is still not missing, but does not
        // exist on disk, bail.
        if (!(__is_missing(opath)) && !std::filesystem::exists(opath))
            return WithState(a, StateErr::DLL_NOMODULE);
        return WithEntry(a, opath);
    }

    Artifact FromTomlWithMode(const Artifact& a, toml::table& tbl) {
        return WithMode(a, __tbl_extract_str(tbl, __fld_mode, __val_none));
    }

    Artifact FromTomlWithName(const Artifact& a, toml::table& tbl) {
        return WithName(a, __tbl_extract_str(tbl, __fld_name, __val_anon));
    }

    Artifact FromTomlWithType(const Artifact& a, toml::table& tbl) {
        return WithType(a, __tbl_extract_str(tbl, __fld_type, __val_dyn));
    }

    Artifact FromTomlWithVersion(const Artifact& a, toml::table& tbl) {
        return WithVersion(a, __tbl_extract_str(tbl, __fld_vers, "0.0.0"));
    }

    // Remove macros not needed beyond this point.
    #undef __tbl_extracts_str

    Artifact FromTomlHeader(const Artifact& a, toml::table& tbl, HeaderExtractors exts) {
        auto amod = FromOther(a);
        for (const auto ext : exts) {
            amod = ext(amod, tbl);
        }
        return amod;
    }

    Artifact FromTomlHeader(const Artifact& a, toml::table& tbl, const std::string& header, HeaderExtractors exts) {
        try {
            if (!tbl[header]) return WithState(a, StateErr::MAN_NOHEADER, header);
            return FromTomlHeader(a, *(tbl[header].as_table()), exts);
        } catch (const toml::parse_error& err) {
            return WithState(a, StateErr::MAN_MALPARSE, err.what());
        }
    }

    Artifact FromTomlHeaders(const Artifact& a, toml::table& tbl) {
        auto amod = FromOther(a);
        amod = FromTomlHeader(amod, tbl, __hdr_plugin, {
            FromTomlWithDescription,
            FromTomlWithDescriptionLong,
            FromTomlWithName,
            FromTomlWithVersion
        });
        amod = FromTomlHeader(amod, tbl, __hdr_module, {
            FromTomlWithEntry,
            FromTomlWithMode,
            FromTomlWithType
        });
        return amod;
    }

    Artifact FromToml(const Artifact& a) {
        try {
            auto tbl = toml::parse_file(a->manifest);
            return FromTomlHeaders(a, tbl);
        } catch (const toml::parse_error& err) {
            return WithState(a, StateErr::MAN_MALPARSE, err.what());
        }
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
        a->obj_type         = source->obj_type;
        a->state            = source->state;
        a->version          = source->version;
        return a;
    }

    toml::table IntoToml(const Artifact& source) {
        toml::table tbl;
        try {
            tbl = toml::parse_file(source->manifest);
        } catch (toml::parse_error& _) {
            // Who cares. We want to return literally
            // anything.
        }
        auto status = toml::table{
            {"code", source->state.code },
            {"detail", source->state.detail},
            {"group", StateGroup<std::string>(source->state.group) },
            {"message", source->state.message },
        };
        tbl.emplace("status", status);

        // All below is reserved for if we can install the
        // the plugin.
        if (!source->IsInstalled()) return tbl;

        toml::array command_names{};
        for (auto it = source->commands.begin(); it != source->commands.end(); it++)
            command_names.push_back(it->first);
        tbl.emplace("commands", command_names);

        return tbl;
    }

    const std::string Repr(const Artifact& source) {
        std::stringstream ss;
        ss << source;
        return std::string(ss.str().c_str());
    }

    const std::string ReprJson(const Artifact& source) {
        auto tbl = IntoToml(source);
        std::stringstream ss;
        ss << toml::json_formatter { tbl };
        return std::string(ss.str().c_str());
    }

    const std::string ReprToml(const Artifact& source) {
        auto tbl = IntoToml(source);
        std::stringstream ss;
        ss << tbl;
        return std::string(ss.str().c_str());
    }

    const std::string ReprYaml(const Artifact& source) {
        auto tbl = IntoToml(source);
        std::stringstream ss;
        ss << toml::yaml_formatter{ tbl };
        return std::string(ss.str().c_str());
    }

    std::ostream& operator<<(std::ostream& os, const Artifact& a) {
        os << "Artifact[";
            os << "name=\"" << a->name << "\", ";
            os << "description=\"" << a->description << "\", ";
            os << "description_long=\"" << a->description_long << "\", ";
            os << "manifest=\"" << a->manifest << "\", ";
            os << "module_entry=\"" << a->obj_entry << "\", ";
            os << "module_mode=" << a->obj_mode << ", ";
            os << "module_ref=" << a->obj_ref << ", ";
            os << "module_type=" << a->obj_type << ", ";

        if (a->IsInstalled()) {
            os << "commands=[";
            auto it = a->commands.begin();
            while (it != a->commands.end()) {
                os << it->first;
                if (++it != a->commands.end()) os << ", ";
            };
            os << "], ";
        }

        os << "state=" << a->state << ", ";
        os << "version=\"" << a->version << "\"";
        return os << "]";
    }
}
