#include <filesystem>
#include <sstream>
#include <vector>

#include <toml++/toml.hpp>

#include "plugin.hpp"
#include "plugin_internal.hpp"

namespace __plugin_root::artifact {
    using Manifest = manifest::Manifest;
    typedef std::function<Artifact(const Artifact&, toml::table&)> HeaderExtractor;
    typedef std::vector<HeaderExtractor> HeaderExtractors;

    bool Body::IsInstalled(void) {
        return state.code == NONE && obj_ref != nullptr;
    }

    void Body::SetState(const StateInfo info) {
        this->state = info;
    }

    const Name Body::GetName(void) {
        return name;
    }

    const Desc Body::GetDescription(void) {
        return description;
    }

    const Desc Body::GetDescriptionLong(void) {
        return description_long;
    }

    const ObjectEntry Body::GetEntry(void) {
        return obj_entry;
    }

    const BuildMode Body::GetMode(void) {
        return obj_mode;
    }

    const BuildType Body::GetType(void) {
        return obj_type;
    }

    Artifact New(void) {
        return std::move(std::make_unique<Body>());
    }

    Artifact New(const ObjectEntry path) {
        auto a = New();
        a->obj_entry = path;
        return a;
    }

    Body::Builder::Builder(void) : inner(std::make_unique<Body>()) {}

    Body::Builder::Builder(std::filesystem::path manifest) : Builder() {
        Manifest m;

        WithManifest(manifest);

        if (!std::filesystem::exists(manifest)) {
            WithState(StateErr::MAN_NOMAN, manifest);
            return;
        }

        try {
            m = Manifest::Load(manifest);
        } catch (const toml::parse_error& err) {
            WithState(StateErr::MAN_MALPARSE, err.what());
            return;
        }

        if (!std::filesystem::exists(m.module.entry))
            WithState(StateErr::DLL_NOMODULE, m.module.entry);

        WithDescription(m.plugin.description);
        WithDescriptionLong(m.plugin.description_long);
        WithEntry(m.module.entry);
        WithMode(m.module.mode);
        WithName(m.plugin.name);
        WithType(m.module.type);
        WithVersion(m.plugin.version);
    }

    Body::Builder::Builder(const Artifact& other) : Builder() {
        inner->commands         = other->commands;
        inner->description      = other->description;
        inner->description_long = other->description_long;
        inner->manifest         = other->manifest;
        inner->name             = other->name;
        inner->obj_entry        = other->obj_entry;
        inner->obj_mode         = other->obj_mode;
        inner->obj_ref          = other->obj_ref;
        inner->obj_type         = other->obj_type;
        inner->state            = other->state;
        inner->version          = other->version;
    }

    Artifact Body::Builder::Build(void) {
        return std::move(inner);
    }

    Body::Builder& Body::Builder::WithDescription(const Desc desc) {
        inner->description = desc;
        return *this;
    }

    Body::Builder& Body::Builder::WithDescriptionLong(const Desc desc) {
        inner->description_long = desc;
        return *this;
    }

    Body::Builder& Body::Builder::WithEntry(const ObjectEntry entry) {
        inner->obj_entry = entry;
        return *this;
    }

    Body::Builder& Body::Builder::WithManifest(const Path path) {
        auto path_p = std::filesystem::path(path);

        try {
            if (path_p.is_relative())
                path_p = std::filesystem::absolute(path_p);
            if (!std::filesystem::exists(path_p))
                return WithState(StateErr::MAN_NOMAN, path_p);
        } catch (const std::filesystem::filesystem_error& err) {
            return WithState(StateErr::MAN_MALPARSE, err.what());
        }

        inner->manifest = path_p.string();
        return *this;
    }

    Body::Builder& Body::Builder::WithMode(const std::string mode) {
        return WithMode(BuildModes.at(mode));
    }

    Body::Builder& Body::Builder::WithMode(const BuildMode mode) {
        inner->obj_mode = mode;
        return *this;
    }

    Body::Builder& Body::Builder::WithName(const Name name) {
        inner->name = name;
        return *this;
    }

    Body::Builder& Body::Builder::WithState(const StateErr state) {
        return WithState(state, "");
    }

    Body::Builder& Body::Builder::WithState(const StateErr state, const StateErrMessage detail) {
        inner->SetState(state & detail);
        return *this;
    }

    Body::Builder& Body::Builder::WithType(const BuildType type) {
        inner->obj_type = type;
        return *this;
    }

    Body::Builder& Body::Builder::WithVersion(const Version version) {
        inner->version = version;
        return *this;
    }

    Artifact FromManifest(const std::filesystem::path path) {
        return Body::Builder(path).Build();
    }

    Artifact FromOther(const Artifact& source) {
        return Body::Builder(source).Build();
    }

    Artifact FromOther(const Body& source) {
        return FromOther(std::make_unique<Body>(source));
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

    const std::string Body::Repr() {
        std::stringstream ss;
        ss << *this;
        return std::string(ss.str().c_str());
    }

    const std::string Body::ReprJson() {
        auto tbl = IntoToml(std::make_unique<Body>(*this));
        std::stringstream ss;
        ss << toml::json_formatter { tbl };
        return std::string(ss.str().c_str());
    }

    const std::string Body::ReprToml() {
        auto tbl = IntoToml(std::make_unique<Body>(*this));
        std::stringstream ss;
        ss << tbl;
        return std::string(ss.str().c_str());
    }

    const std::string Body::ReprYaml() {
        auto tbl = IntoToml(std::make_unique<Body>(*this));
        std::stringstream ss;
        ss << toml::yaml_formatter{ tbl };
        return std::string(ss.str().c_str());
    }

    std::ostream& operator<<(std::ostream& os, const Artifact& a) {
        return os << *a;
    }

    std::ostream& operator<<(std::ostream& os, const Body& a) {
        os << "Artifact[";
            os << "name=\"" << a.name << "\", ";
            os << "description=\"" << a.description << "\", ";
            os << "description_long=\"" << a.description_long << "\", ";
            os << "manifest=\"" << a.manifest << "\", ";
            os << "module_entry=\"" << a.obj_entry << "\", ";
            os << "module_mode=" << a.obj_mode << ", ";
            os << "module_ref=" << a.obj_ref << ", ";
            os << "module_type=" << a.obj_type << ", ";

        os << "commands=[";
        auto it = a.commands.begin();
        while (it != a.commands.end()) {
            os << it->first;
            if (++it != a.commands.end()) os << ", ";
        };
        os << "], ";

        os << "state=" << a.state << ", ";
        os << "version=\"" << a.version << "\"";
        return os << "]";
    }
}
