#include <functional>
#include <optional>

#include <CLI/CLI.hpp>

struct Options {
    std::string manifest;
};

struct DefinitionContext {
    CLI::App&   parent;
    Options&    common;
};

void define_opt_common(DefinitionContext& ctx) {
    ctx.parent.add_option("-M,--manifest", ctx.common.manifest, "Manifest file")->default_val("plugin.toml");
}

void define_cmd_new(DefinitionContext ctx, const std::string& name = "", const std::string& desc = "", std::optional<std::function<void(CLI::App&)>> fn = {}) {
    CLI::App *app = ctx.parent.add_subcommand(name, desc);
    DefinitionContext app_ctx = {
        .parent = *app,
        .common = ctx.common
    };
    define_opt_common(app_ctx);
    if (fn.has_value()) fn.value()(*app);
}

int main(int argv, const char **argc) {
    CLI::App app("developer tool for yengine plugins", "yplugin");
    Options  opt = {};
    DefinitionContext app_ctx = {
        .parent = app,
        .common = opt,
    };

    define_cmd_new(app_ctx, "build", "build a yengine plugin");
    define_cmd_new(app_ctx, "inspect", "inspect a yengine plugin");

    CLI11_PARSE(app, argv, argc)
    return 0;
}
