#include <filesystem>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>

#include "plugin.hpp"
#include "plugin_internal.hpp"

namespace __plugin_root {
    Context Install(const char *path) {
        return Install({ artifact::FromFile(path) });
    }

    Context Install(const Context& ctx) {
        return dllutil::InstallModule(ctx)
            .and_then(dllutil::InstallCommand("OnInstall",    command::DefaultPassthrough))
            .and_then(dllutil::InstallCommand("OnRelease",    command::DefaultPassthrough))
            .and_then(dllutil::InstallCommand("OnInitialize", command::DefaultPassthrough))
            .and_then(dllutil::CallCommand("OnInstall"))
            .or_else(dllutil::HandleErr(ctx))
            .value();
    }

    Context Initialize(const Context& ctx) {
        return dllutil::CallCommand(ctx, "OnInitialize")
            .or_else(dllutil::HandleErr(ctx))
            .value();
    }

    Context Release(const Context& ctx) {
        return dllutil::CallCommand(ctx, "OnRelease")
            .or_else(dllutil::HandleErr(ctx))
            .and_then(dllutil::ReleaseModule)
            .value();
    }

    StateInfo::StateInfo(void) {
        code    = NONE;
        group   = StateGroup<StateErrGroup>(NONE);
        message = StateMessage(NONE);
    }

    StateInfo::StateInfo(const StateErr& s) {
        code    = s;
        group   = StateGroup<StateErrGroup>(s);
        message = StateMessage(s);
    }

    StateInfo::StateInfo(const StateErr& s, const StateErrMessage& d) {
        code    = s;
        detail  = d;
        group   = StateGroup<StateErrGroup>(s);
        message = StateMessage(s);
    }

    template <>
    const std::string StateGroup(const StateErrGroup& group) {
        return StateErrGroupNames.at(group);
    }

    template <>
    const std::string StateGroup(const StateErr& state) {
        return StateGroup<std::string>(StateGroup<StateErrGroup>(state));
    }

    template <>
    const StateErrGroup StateGroup(const StateErr& state) {
        return (StateErrGroup)(state / StateErrGroupOrder);
    }

    const StateErrMessage StateMessage(const StateErr& state) {
        return StateErrMessages.at(state);
    }

    StateInfo operator*(StateErr state) {
        return StateInfo(state);
    }

    StateInfo operator&(StateErr state, StateErrMessage detail) {
        return StateInfo(state, detail);
    }

    std::ostream& operator<<(std::ostream& os, const StateErr& state) {
        return os << *state;
    }

    StateErr operator*(StateInfo info) {
        return info.code;
    }

    bool operator==(StateInfo info, StateErr state) {
        return info.code == state;
    }

    std::ostream& operator<<(std::ostream& os, const StateInfo& info) {
        return os << "<"
            << (std::size_t)info.code << ":"
            << StateGroup<std::string>(info.group) << ":\""
            << info.message
            << "\">";
    }

    StateErr CallCommand(const Context *ctx, const char *name) {
        return dllutil::CallCommand(*ctx, name).error_or(NONE);
    }

    const char *GetName(const Context *ctx) {
        return ctx->artifact->name.c_str();
    }

    const char *GetDescription(const Context *ctx) {
        return ctx->artifact->description.c_str();
    }

    const char *GetDescriptionLong(const Context *ctx) {
        return ctx->artifact->description_long.c_str();
    }

    StateErr RegisterCommand(const Context *ctx, const char *name, const char *impl_name) {
        return dllutil::InstallCommand(*ctx, name, impl_name).error_or(NONE);
    }
}
