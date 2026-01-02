#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <sstream>

#include "plugin.hpp"

// Must be included before CLI11.hpp
namespace options {
    typedef enum InspectFormat {
        UNKNOWN = -1,
        JSON,
        REPR,
        TOML,
        YAML
    } InspectFormat;

    const std::map<std::string, InspectFormat> format_types {
        {"json", InspectFormat::JSON},
        {"repr", InspectFormat::REPR},
        {"toml", InspectFormat::TOML},
        {"yaml", InspectFormat::YAML},
    };

    InspectFormat FromStr(const std::string value) {
        if (!format_types.contains(value)) return InspectFormat::UNKNOWN;
        return format_types.at(value);
    }

    const std::string IntoStr(const InspectFormat value) {
        switch (value) {
            case JSON: return "json";
            case REPR: return "repr";
            case TOML: return "toml";
            case YAML: return "yaml";
            default: return "unknown";
        }
    }

    struct Inspect {
        InspectFormat format;
    };

    struct Common {
        std::string manifest;
        Inspect     on_inspect;
    };
}

namespace CLI {
    std::istringstream& operator>>(std::istringstream &in, options::InspectFormat& fmt) {
        std::string temp;
        in >> temp;
        fmt = options::FromStr(temp);
        return in;
    }

    std::stringstream& operator<<(std::stringstream& ss, options::InspectFormat& fmt) {
        ss << options::IntoStr(fmt);
        return ss;
    }
}

#include <CLI/CLI.hpp>

namespace CLI::detail {
    template<>
    constexpr const char *type_name<options::InspectFormat>() {
        return "FORMAT";
    }
}

namespace define {
    typedef std::optional<std::function<void(CLI::App&)>> CmdMutation;

    struct Context {
        #define __Context_attrs \
            options::Common& common; \
            std::string      cmd_name; \
            std::string      cmd_desc; \
            CmdMutation      cmd_mutation;
        __Context_attrs

        static Context New(options::Common& opt) {
            return Builder(opt).Build();
        }

        CLI::App& NewCommand(CLI::App& parent) {
            CLI::App *app = parent.add_subcommand(cmd_name, cmd_desc);
            ApplyOptCommon(*app);
            ApplyMutation(*app);
            return *app;
        }

        Context& ApplyInspect(CLI::App& cmd) {
            cmd.add_option("-f,--format", common.on_inspect.format, "format output")->default_str("toml");
            return *this;
        }

        Context& ApplyOptCommon(CLI::App& cmd) {
            cmd.add_option(
                "-M,--manifest",
                common.manifest,
                "Manifest file"
            )->default_val("plugin.toml");
            return *this;
        }

        Context& ApplyMutation(CLI::App& cmd) {
            if (cmd_mutation.has_value()) cmd_mutation.value()(cmd);
            return *this;
        }

        class Builder {
            #define __ContextBuilder_FromOtherPtrInits \
                common(other->common), \
                cmd_desc(other->cmd_desc), \
                cmd_mutation(other->cmd_mutation), \
                cmd_name(other->cmd_name)
            public:
            Builder(Context *other) : __ContextBuilder_FromOtherPtrInits {}
            Builder(Builder *other) : __ContextBuilder_FromOtherPtrInits {}
            Builder(options::Common& opt) : common(opt) {}

            Context Build(void) {
                Context ctx = {
                    .common       = common,
                    .cmd_desc     = cmd_desc,
                    .cmd_mutation = cmd_mutation,
                    .cmd_name     = cmd_name
                };
                return ctx;
            }

            Builder& WithCmdName(const char *value) {
                this->cmd_name = value;
                return *this;
            }

            Builder& WithCmdDesc(const char *value) {
                this->cmd_desc = value;
                return *this;
            }

            Builder& WithCmdMut(const CmdMutation fn) {
                this->cmd_mutation = fn;
                return *this;
            }

            private:
            __Context_attrs

        };
    };
}

auto CmdBuild(define::Context& ctx) {
    return [&ctx](){
        auto artifact = plugin::artifact::FromFile(ctx.common.manifest.c_str());
        auto state = plugin::package::Build(artifact);
        if (state != plugin::NONE) {
            std::cerr << "error: " << state << std::endl;
            std::exit(1);
        }
    };
}

auto CmdInspect(define::Context& ctx) {
    return [ctx](){
        auto pctx = plugin::Install(ctx.common.manifest.c_str());

        std::string out;
        switch (ctx.common.on_inspect.format) {
            case options::JSON:
                out = plugin::artifact::ReprJson(pctx.artifact);
                break;
            case options::TOML:
                out = plugin::artifact::ReprToml(pctx.artifact);
                break;
            case options::REPR:
                out = plugin::artifact::Repr(pctx.artifact);
                break;
            case options::YAML:
                out = plugin::artifact::ReprYaml(pctx.artifact);
                break;
            default:
                return;
        }
        std::cout << out << std::endl;

        if (pctx.artifact->IsInstalled()) plugin::Release(pctx);
    };
}

int main(int argv, const char **argc) {
    CLI::App app("developer tool for yengine plugins", "yplugin");
    options::Common opt = {};

    auto app_ctx = define::Context::New(opt);
    define::Context::Builder(&app_ctx)
        .WithCmdName("build")
        .WithCmdDesc("build a yengine plugin")
        .Build()
        .NewCommand(app)
        .callback(CmdBuild(app_ctx));
    auto inspect = define::Context::Builder(&app_ctx)
        .WithCmdName("inspect")
        .WithCmdDesc("inspect a yengine plugin")
        .Build()
        .NewCommand(app)
        .callback(CmdInspect(app_ctx));
    inspect
        ->add_option("-f,--format", opt.on_inspect.format, "format output")
        ->default_val(options::InspectFormat::JSON)
        ->transform(CLI::CheckedTransformer(options::format_types));

    CLI11_PARSE(app, argv, argc)
    return 0;
}
