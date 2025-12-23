#include "Plugin.hpp"
#include "State.hpp"
#include "Traits.hpp"

using namespace plugin;

int main(void) {
    plugin::PluginManager manager;
    #ifdef __APPLE__
    manager.RegisterArtifact("HelloWorld", "/Users/kwilkinson/dev/experimental/engine/targets/macosx/arm64/release/libhello_world.dylib");
    #elif __unix__
    manager.RegisterArtifact("HelloWorld", "/workspaces/engine/build/linux/arm64-v8a/release/libhello_world.so");
    #endif
    manager.LoadArtifacts();
    manager.ReleaseArtifacts();
    return 0;
}
