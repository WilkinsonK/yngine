#include "plugin.hpp"
#include "plugin_internal.hpp"

namespace plugin::command {
    using Artifact = artifact::Artifact;

    State<CommandImpl*> LoadCommandImplFromArtifact(const Artifact& source, const Name name) {
        return dllutil::LoadHandleFromArtifact(source, name + "_callbackImpl")
            .transform_error([](StateErr err) {
                if (err == DLL_NOMODULE) return err;
                return DLL_NOCALLBACK;
            })
            .transform([](void *handle){
                return (CommandImpl*)(handle);
            });
    }

    State<CommandScope*> LoadCommandScopeFromArtifact(const Artifact& source, const Name name) {
        return dllutil::LoadHandleFromArtifact(source, name + "_callbackScope")
            .transform_error([](StateErr err) {
                if (err == DLL_NOMODULE) return err;
                return DLL_NOSCOPE;
            })
            .transform([](void *handle){
                return (CommandScope*)(handle);
            });
    }

    Command LoadFromArtifact(const Artifact& source, Name name) {
        Command cmd = {0};

        auto scope = LoadCommandScopeFromArtifact(source, name);
        if (!scope.has_value()) {
            cmd.state = scope.error();
            return cmd;
        }

        auto impl = LoadCommandImplFromArtifact(source, name);
        if (!impl.has_value()) {
            cmd.state = impl.error();
            return cmd;
        }

        cmd.impl = *impl.value();
        cmd.scope = *scope.value();
        return cmd;
    }
}