#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <sstream>

#include "CLI/CLI.hpp"

#include "plugin.hpp"

typedef enum Format {
    F_UNKNOWN = -1,
    F_JSON,
    F_REPR,
    F_TOML,
    F_YAML
} Format;

const std::map<std::string, Format> format_types {
    {"json", Format::F_JSON},
    {"repr", Format::F_REPR},
    {"toml", Format::F_TOML},
    {"yaml", Format::F_YAML},
};

struct Options {
    Format      format;
    std::string manifest_name;
    std::string pkgdir;
    std::string pkgout;
};

CLI::App *subcommand(CLI::App *root, const std::string& name, const std::string desc) {
    return root->add_subcommand(name, desc);
}

void add_format_option(CLI::App *app, Options *opts) {
    app ->add_option("-f,--format", opts->format, "format output")
        ->default_val(Format::F_JSON)
        ->transform(CLI::CheckedTransformer(format_types));
}

void add_manifest_option(CLI::App *app, Options *opts) {
    app ->add_option("-M,--manifest", opts->manifest_name, "manifest file")
        ->default_val("plugin.toml");
}

void add_pkgdir_option(CLI::App *app, Options *opts) {
    app->add_option("-D,--pkg-dir", opts->pkgdir, "package directory path");
}

void add_pkgout_option(CLI::App *app, Options *opts) {
    app->add_option("-O,--pkg-out", opts->pkgout, "package build output directory");
}

auto cmd_callback_build(Options *opts) {
    return [opts](){
        auto pkg = plugin::package::Builder()
            .WithManifestName(opts->manifest_name)
            .WithOutputDir(opts->pkgout)
            .WithSourceDir(opts->pkgdir)
            .Build();
        auto ctx = plugin::Context::FromManifest(pkg->GetManifest());

        auto state = plugin::package::Build(ctx);
        if (state != plugin::NONE) goto handle_err;
        plugin::package::Pack(ctx);
        if (state != plugin::NONE) goto handle_err;

        return;
        handle_err:
        std::cerr << "error: " << state.message << " '" << state.detail << "'" << std::endl;
        std::exit(1);
    };
}

auto cmd_callback_inspect(Options *opts) {
    return [opts](){
        auto ctx = plugin::Install(opts->manifest_name.c_str());

        std::string out;
        switch (opts->format) {
            case F_JSON:
                out = ctx.artifact->ReprJson();
                break;
            case F_TOML:
                out = ctx.artifact->ReprToml();
                break;
            case F_REPR:
                out = ctx.artifact->Repr();
                break;
            case F_YAML:
                out = ctx.artifact->ReprYaml();
                break;
            default:
                return;
        }
        std::cout << out << std::endl;

        if (ctx.artifact->IsInstalled()) plugin::Release(ctx);
    };
}

int main(int argv, const char **argc) {
    CLI::App app("developer tool for yengine plugins", "yplugin");
    Options opts;

    auto cmd_build = subcommand(&app, "build", "build a plugin");
    add_manifest_option(cmd_build, &opts);
    add_pkgdir_option(cmd_build, &opts);
    add_pkgout_option(cmd_build, &opts);
    cmd_build->callback(cmd_callback_build(&opts));

    auto cmd_inspect = subcommand(&app, "inspect", "inspect a plugin");
    add_manifest_option(cmd_inspect, &opts);
    add_format_option(cmd_inspect, &opts);
    cmd_inspect->callback(cmd_callback_inspect(&opts));

    CLI11_PARSE(app, argv, argc)
    return 0;
}
