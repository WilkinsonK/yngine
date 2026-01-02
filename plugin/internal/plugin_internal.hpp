/// @file plugin_internal.hpp
/// @brief Plugin API declarations. Defines type
/// aliases, traits and functions that are privately
/// available to the CXX interface.

#pragma once

#include <toml++/toml.hpp>

#include "plugin.hpp"

namespace __plugin_root::dllutil {
    using artifact::Artifact;
    using command::CommandImplDefault;

    /// @brief Calls a command directly from the artifact.
    /// @param  ctx context of the call being made.
    /// @param  name of the command to be called.
    /// @return The context passed.
    State<Context> CallCommand(const Context&, Name);
    StateFn<Context, const Context&> CallCommand(Name);

    /// @brief Handles errors for DLL handling API.
    /// @param  ctx context of the artifact.
    /// @param  err that occured.
    /// @return The context passed.
    State<Context> HandleErr(const Context&, StateErr);
    StateFn<Context, StateErr> HandleErr(const Context&);

    /// @brief Installs a command from the DLL, making it
    /// available to the artifact.
    /// @param  ctx context of the artifact.
    /// @param  name of the command being installed.
    /// @return The context passed.
    State<Context> InstallCommand(const Context&, Name);
    /// @brief Installs a command from the DLL, making it
    /// available to the artifact.
    /// @param  ctx context of the artifact.
    /// @param  name of the command being installed.
    /// @param  impl_name as the identifier of the command
    /// implementation.
    /// @return The context passed.
    State<Context> InstallCommand(const Context&, Name, Name);
    /// @brief Constructs a command installer.
    /// @param name of the command being installed.
    /// @return A function which installs a command with the
    /// designated name.
    StateFn<Context, const Context&> InstallCommand(Name);
    /// @brief Constructs a command installer.
    /// @param name of the command being installed.
    /// @param impl_default implementation used when command
    /// is not found.
    /// @return A function which installs a command with the
    /// designated name.
    StateFn<Context, const Context&> InstallCommand(Name, CommandImplDefault);
    /// @brief Installs the DLL module handle.
    /// @param  ctx context of the artifact.
    /// @return The context passed.
    State<Context> InstallModule(const Context&);

    /// @brief Loads the DLL handle of some member of the
    /// artifact module.
    /// @param  source artifact to load handle from.
    /// @param  name of the handle being loaded.
    /// @return Ambiguous pointer to the handle.
    State<void*> LoadHandleFromArtifact(const Artifact&, const Name);
    StateFn<void*, const Artifact&> LoadHandleFromArtifact(Name);

    /// @brief Load the module belonging to an artifact.
    /// @param  source artifact to load the module from.
    /// @return The artifact with a loaded module.
    State<void*> LoadModuleFromArtifact(const Artifact&);

    /// @brief Release the DLL module handle and all related
    /// assets.
    /// @param ctx context of the artifact.
    /// @return The context passed.
    State<Context> ReleaseModule(const Context&);
}
