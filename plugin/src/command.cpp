#include "plugin.hpp"
#include "plugin_internal.hpp"

namespace __plugin_root::command {
    using Artifact = artifact::Artifact;

    State<CommandImpl*> LoadCommandImplFromArtifact(const Artifact& source, const Name name) {
        return dllutil::LoadHandleFromArtifact(source, name + "_commandImpl")
            .transform_error([](StateErr err) {
                if (err == DLL_NOMODULE) return err;
                return DLL_NOCALLBACK;
            })
            .transform([](void *handle){
                return (CommandImpl*)(handle);
            });
    }

    State<CommandScope*> LoadCommandScopeFromArtifact(const Artifact& source, const Name name) {
        return dllutil::LoadHandleFromArtifact(source, name + "_commandScope")
            .transform_error([](StateErr err) {
                if (err == DLL_NOMODULE) return err;
                return DLL_NOSCOPE;
            })
            .transform([](void *handle){
                return (CommandScope*)(handle);
            });
    }

    Command LoadFromArtifact(const Artifact& source, const Name name, const Name impl_name) {
        auto scope = LoadCommandScopeFromArtifact(source, impl_name);
        if (!scope.has_value()) {
            return { .impl_name = impl_name, .state = scope.error() };
        }

        auto impl = LoadCommandImplFromArtifact(source, impl_name);
        if (!impl.has_value()) {
            return { .impl_name = impl_name, .state = scope.error() };
        }

        Command cmd = {
            .impl_name = impl_name,
            .impl = *impl.value(),
            .scope = *scope.value()
        };
        return cmd;
    }
}
