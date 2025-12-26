#include "plugin.hpp"

int main(void) {
    auto art = plugin::artifact::FromFile("./plugins/hello_world/plugin.toml");

    art = plugin::dllutil::Install(art);
    if (art->state != plugin::StateErr::NONE) {
        std::cerr << "error: " << art->state << std::endl;
        return 1;
    }

    art = plugin::dllutil::Release(art);
    if (art->state != plugin::StateErr::NONE) {
        std::cerr << "error: " << art->state << std::endl;
        return 1;
    }

    return 0;
}
