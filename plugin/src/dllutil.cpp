#include <filesystem>
#include <functional>

#include <dlfcn.h>

#include "plugin.hpp"
#include "plugin_internal.hpp"

namespace plugin::dllutil {
    using Artifact = artifact::Artifact;

    State<Artifact> CallCommand(const Artifact& source, Name name) {
        if (source->commands[name].state == NONE) {
            source->state = source->commands[name].impl(nullptr);
            return artifact::FromOther(source);
        }
        return std::unexpected(source->commands[name].state);
    }

    std::function<State<Artifact>(const Artifact&)> CallCommand(Name name) {
        return [name](const Artifact& source){
            return CallCommand(source, name);
        };
    }

    State<Artifact> HandleErr(const Artifact& source, StateErr err) {
        source->state = err;
        return artifact::FromOther(source);
    }

    std::function<State<Artifact>(StateErr)> HandleErr(const Artifact& source) {
        return [&source](StateErr err){
            return HandleErr(source, err);
        };
    }

    State<Artifact> InstallCommand(const Artifact& source, Name name) {
        auto a = artifact::FromOther(source);
        a->commands[name] = command::LoadFromArtifact(a, name);
        if (a->commands[name].state != NONE) {
            a->state = a->commands[name].state;
            return std::unexpected(a->state);
        }
        return a;

    }

    std::function<State<Artifact>(const Artifact&)> InstallCommand(Name name) {
        return [name](const Artifact& source){
            return InstallCommand(source, name);
        };
    }

    State<Artifact> InstallModule(const Artifact& source) {
        auto a = artifact::FromOther(source);
        return LoadModuleFromArtifact(a)
            .transform_error([&a](StateErr err){
                a->state = err;
                return err;
            })
            .transform([&a](void *module){
                a->obj_ref = module;
                return std::move(a);
            });
    }

    State<void*> LoadHandleFromArtifact(const Artifact& source, const Name name) {
        if (source->state != NONE) return std::unexpected(DLL_NOMODULE);
        void *handle = dlsym(source->obj_ref, name.c_str());
        if (handle == nullptr) return std::unexpected(DLL_NOOBJECT);
        return handle;
    }

    std::function<State<void*>(const Artifact&)> LoadHandleFromArtifact(Name name) {
        return [name](const Artifact& source){
            return LoadHandleFromArtifact(source, name);
        };
    }

    State<void*> LoadModuleFromArtifact(const Artifact& source) {
        if (!std::filesystem::exists(source->obj_path)) return std::unexpected(DLL_NOMODULE);
        void *module = dlopen(source->obj_path.c_str(), source->obj_mode);
        if (module == nullptr) return std::unexpected(DLL_MALFORMAT);
        return module;
    }
}
