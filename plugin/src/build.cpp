#include "plugin.hpp"
#include "plugin_internal.hpp"

namespace __plugin_root::package {
    typedef std::function<StateErr(const artifact::Artifact&)> BuildModeHandler;

    StateErr BuildWithMake(const artifact::Artifact& source) {
        return NONE;
    }

    StateErr Build(const artifact::Artifact& source, BuildModeHandler handler) {
        return handler(source);
    }

    StateErr Build(const artifact::Artifact& source) {
        // Build the module according to build mode.
        // Check that the build was sucessful.
        // Check that the resulting entry point matches manifest.
        if (artifact::GetMode(source) == "make") return Build(source, BuildWithMake);
        return NONE;
    }
}
