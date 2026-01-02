#include <filesystem>
#include <functional>

#include <dlfcn.h>

#include "plugin.hpp"
#include "plugin_internal.hpp"

namespace __plugin_root::dllutil {
    State<Context> CallCommand(const Context& ctx, Name name) {
        auto cmd = ctx.artifact->commands[name];
        if (cmd.state == NONE) {
            ctx.artifact->SetState(*cmd.impl(&ctx));
            Context newctx = { artifact::FromOther(ctx.artifact) };
            return newctx;
        }
        std::cerr << "error: failed to call command '" << name << "': " << ctx.artifact->commands[name].state << std::endl;
        return std::unexpected(*ctx.artifact->commands[name].state);
    }

    StateFn<Context, const Context&> CallCommand(Name name) {
        return [name](const Context& ctx){
            return CallCommand(ctx, name);
        };
    }

    State<Context> HandleErr(const Context& ctx, StateErr err) {
        ctx.artifact->SetState(*err);
        Context newctx = { artifact::FromOther(ctx.artifact) };
        return newctx;
    }

    StateFn<Context, StateErr> HandleErr(const Context& ctx) {
        return [&ctx](StateErr err){
            return HandleErr(ctx, err);
        };
    }

    State<Context> InstallCommand(const Context& ctx, Name name, Name impl_name, CommandImplDefault impl_default) {
        auto cmd = ctx.artifact->commands[name] = command::LoadFromArtifact(ctx.artifact, name, impl_name, impl_default);
        if (cmd.state != NONE) {
            ctx.artifact->SetState(cmd.state);
            return std::unexpected(*ctx.artifact->state);
        }

        Context newctx = { artifact::FromOther(ctx.artifact) };
        return newctx;
    }

    State<Context> InstallCommand(const Context& ctx, Name name, Name impl_name) {
        return InstallCommand(ctx, name, impl_name, {});
    }

    State<Context> InstallCommand(const Context& ctx, Name name) {
        return InstallCommand(ctx, name, name, {});
    }

    StateFn<Context, const Context&> InstallCommand(Name name) {
        return [name](const Context& ctx){
            return InstallCommand(ctx, name, name, {});
        };
    }

    StateFn<Context, const Context&> InstallCommand(Name name, CommandImplDefault impl_default) {
        return [name, impl_default](const Context& ctx){
            return InstallCommand(ctx, name, name, impl_default);
        };
    }

    State<Context> InstallModule(const Context& ctx) {
        return LoadModuleFromArtifact(ctx.artifact)
            .transform_error([&ctx](StateErr err){
                ctx.artifact->state = *err;
                return err;
            })
            .transform([&ctx](void *module){
                ctx.artifact->obj_ref = module;
                Context newctx = { artifact::FromOther(ctx.artifact) };
                return newctx;
            });
    }

    State<void*> LoadHandleFromArtifact(const Artifact& source, const Name name) {
        if (*source->state != NONE) return std::unexpected(DLL_NOMODULE);
        void *handle = dlsym(source->obj_ref, name.c_str());
        if (handle == nullptr) return std::unexpected(DLL_NOOBJECT);
        return handle;
    }

    StateFn<void*, const Artifact&> LoadHandleFromArtifact(Name name) {
        return [name](const Artifact& source){
            return LoadHandleFromArtifact(source, name);
        };
    }

    State<void*> LoadModuleFromArtifact(const Artifact& source) {
        if (!std::filesystem::exists(source->obj_entry)) return std::unexpected(DLL_NOMODULE);
        void *module = dlopen(source->obj_entry.c_str(), RTLD_LAZY);
        if (module == nullptr) return std::unexpected(DLL_MALFORMAT);
        return module;
    }

    State<Context> ReleaseModule(const Context& ctx) {
        ctx.artifact->commands.clear();
        dlclose(ctx.artifact->obj_ref);
        ctx.artifact->obj_ref = nullptr;
        Context newctx = { artifact::FromOther(ctx.artifact) };
        return newctx;
    }
}
