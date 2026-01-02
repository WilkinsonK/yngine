#include "plugin.hpp"
#include "plugin_internal.hpp"

namespace __plugin_root::command {
    using Artifact = artifact::Artifact;

    StateErr DefaultPassthrough(const Context *_) {
        return NONE;
    }

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
        return LoadFromArtifact(source, name, impl_name, {});
    }

    Command LoadFromArtifact(const Artifact& source, const Name name, const Name impl_name, CommandImplDefault impl_default) {
        auto scope = LoadCommandScopeFromArtifact(source, impl_name);
        if (!scope.has_value() && !impl_default.has_value()) {
            return { .impl_name = impl_name, .state = scope.error() };
        }

        auto impl = LoadCommandImplFromArtifact(source, impl_name);
        if (!impl.has_value() && !impl_default.has_value()) {
            return { .impl_name = impl_name, .state = scope.error() };
        }

        CommandScope scope_to_use = scope.has_value() ? *scope.value() : SCOPE_LOCAL;
        CommandImpl  impl_to_use = impl.has_value() ? *impl.value() : impl_default.value();

        Command cmd = {
            .impl_name = impl_name,
            .impl      = impl_to_use,
            .scope     = scope_to_use
        };
        return cmd;
    }
}
