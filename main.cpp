#include "plugin.hpp"

int run_plugin_test(const char *path) {
    std::cout << "Trying '" << path << "'" << std::endl;
    auto art = plugin::artifact::FromFile(path);

    plugin::Context ctx = {
        std::move(art)
    };

    ctx = plugin::Install(ctx);
    if (ctx.artifact->state != plugin::StateErr::NONE) {
        std::cerr << "error: " << ctx.artifact->state << std::endl;
        return 1;
    }

    ctx = plugin::Release(ctx);
    if (ctx.artifact->state != plugin::StateErr::NONE) {
        std::cerr << "error: " << ctx.artifact->state << std::endl;
        return 1;
    }

    return 0;
}

int main(void) {
    std::cout << "Testing plugin applications" << std::endl;
    run_plugin_test("/Users/kwilkinson/dev/experimental/engine/plugins/hello_cxx/plugin.toml");
    run_plugin_test("/Users/kwilkinson/dev/experimental/engine/plugins/hello_c/plugin.toml");
    return 0;
}
