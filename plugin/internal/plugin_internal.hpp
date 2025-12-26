/// @file plugin_internal.hpp
/// @brief Plugin API declarations. Defines type
/// aliases, traits and functions that are privately
/// available to the CXX interface.

#pragma once

#include <functional>

#include "plugin.hpp"

namespace plugin::dllutil {
    using Artifact = artifact::Artifact;
    /// @brief Calls a command directly from the artifact.
    /// @param  source artifact command belongs to.
    /// @param  name of the command to be called.
    /// @return The artifact result.
    State<Artifact> CallCommand(const Artifact&, Name);
    std::function<State<Artifact>(const Artifact&)> CallCommand(Name);
    /// @brief Handles errors for DLL handling API.
    /// @param  source artifact being manipulated.
    /// @param  err that occured.
    /// @return The artifact result.
    State<Artifact> HandleErr(const Artifact&, StateErr);
    std::function<State<Artifact>(StateErr)> HandleErr(const Artifact&);
    /// @brief Installs a command from the DLL, making it
    /// available to the artifact.
    /// @param  source artifact command is being installed
    /// from and to.
    /// @param  name of the command being installed.
    /// @return The artifact with installed command.
    State<Artifact> InstallCommand(const Artifact&, Name);
    std::function<State<Artifact>(const Artifact&)> InstallCommand(Name);
    /// @brief Installs the DLL module handle.
    /// @param  source artifact being installed to.
    /// @return The artifact with installed module.
    State<Artifact> InstallModule(const Artifact&);
    /// @brief Loads the DLL handle of some member of the
    /// artifact module.
    /// @param  source artifact to load handle from.
    /// @param  name of the handle being loaded.
    /// @return Ambiguous pointer to the handle.
    State<void*> LoadHandleFromArtifact(const Artifact&, const Name);
    std::function<State<void*>(const Artifact&)> LoadHandleFromArtifact(Name);
    /// @brief Load the module belonging to an artifact.
    /// @param  source artifact to load the module from.
    /// @return The artifact with a loaded module.
    State<void*> LoadModuleFromArtifact(const Artifact&);
}
