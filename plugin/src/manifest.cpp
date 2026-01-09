#include "plugin.hpp"
#include "plugin_internal.hpp"

namespace __plugin_root::manifest {
    // This operation is only needed as a macro as it is
    // repeatable, but not worth adding on to the call
    // stack. Nor is it a viable option to make this an
    // inline function as the overhead isn't worth it.
    #define __tbl_extract_str(T, NAME, DEFAULT) T[NAME].value_or<std::string>(DEFAULT)
    #define __tbl_extract_paths(T, NAME, DEFAULT) T[NAME].value_or<Paths>(DEFAULT)
    #define __fld_desc    "description"
    #define __fld_descl   "description_long"
    #define __fld_entry   "entry"
    #define __fld_include "include"
    #define __fld_mode    "mode"
    #define __fld_name    "name"
    #define __fld_type    "type"
    #define __fld_vers    "version"
    #define __hdr_plugin  "plugin"
    #define __hdr_module  "module"
    #define __val_anon    "anonymous"
    #define __val_dyn     "dynamic"
    #define __val_empty   {}
    #define __val_miss    "missing"
    #define __val_none    "none"

    #define __is_missing(str) str == __val_miss

    Manifest Manifest::New(void) {
        return {};
    }

    Manifest Manifest::New(const toml::table& tbl) {
        auto man = Manifest::New();
        man.plugin = Plugin::New();
        man.module = Module::New();

        if (tbl[__hdr_plugin])
            man.plugin = Plugin::New(*(tbl[__hdr_plugin].as_table()));
        if (tbl[__hdr_module])
            man.module = Module::New(*(tbl[__hdr_module].as_table()));

        return man;
    }

    Manifest Manifest::Load(const std::filesystem::path source) {
        auto m = Manifest::New(toml::parse_file(source.string()));
        if (!(__is_missing(m.module.entry)) && m.module.entry.is_relative())
            m.module.entry = source.parent_path() / m.module.entry;
        return m;
    }

    Module Module::New(void) {
        return {};
    }

    Module Module::New(const toml::table& tbl) {
        auto mod = Module::New();

        mod.entry   = __tbl_extract_str(tbl, __fld_entry, __val_miss);
        mod.include = {};
        mod.mode    = BuildModes.at(__tbl_extract_str(tbl, __fld_mode, "make"));
        mod.type    = BuildTypes.at(__tbl_extract_str(tbl, __fld_type, __val_dyn));

        auto files = tbl[__fld_include].as_array();
        if (!files) return mod;

        for (auto it = files->begin(); it != files->end(); it++)
            mod.include.push_back(*((*it).value<std::string>()));
        return mod;
    }

    Plugin Plugin::New(void) {
        return {};
    }

    Plugin Plugin::New(const toml::table& tbl) {
        auto plg = Plugin::New();
        plg.description      = __tbl_extract_str(tbl, __fld_desc, __val_none);
        plg.description_long = __tbl_extract_str(tbl, __fld_descl, __val_none);
        plg.name             = __tbl_extract_str(tbl, __fld_name, __val_anon);
        plg.version          = __tbl_extract_str(tbl, __fld_vers, "0.0.0");
        return plg;
    }
}
