#include "plugin.hpp"

int main(void) {
    auto art = plugin::artifact::FromFile("/Users/kwilkinson/dev/experimental/engine/plugins/hello_world/plugin.toml");

    plugin::Context ctx = {
        std::move(art)
    };
    std::cout << ctx.artifact << std::endl;

    ctx = plugin::Install(ctx);
    if (ctx.artifact->state != plugin::StateErr::NONE) {
        std::cerr << "error: " << art->state << std::endl;
        return 1;
    }

    ctx = plugin::Release(ctx);
    if (ctx.artifact->state != plugin::StateErr::NONE) {
        std::cerr << "error: " << art->state << std::endl;
        return 1;
    }

    return 0;
}
