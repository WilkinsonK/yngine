/// @file plugin.hpp
/// @brief Main plugin API declarations. Defines type
/// aliases, traits and functions that are publicly
/// available to the CXX interface.

#pragma once

#include <expected>
#include <functional>
#include <iostream>
#include <memory>
#include <unordered_map>

#include "plugin.h"

namespace __plugin_root {
    /// @brief The description of a plugin asset, module or
    /// artifact.
    typedef std::string Desc;
    /// @brief The name of a plugin asset, module or
    /// artifact.
    typedef std::string Name;
    /// @brief Path to the object entry point.
    typedef std::string ObjectEntry;
    /// @brief Mode of how to load the entry point.
    typedef int ObjectMode;
    /// @brief A reference to the object loaded dynamically.
    typedef void *ObjectRef;
    /// @brief Path to an arbitrary file or directory.
    typedef std::string Path;
    /// @brief The version name of a plugin asset, module or
    /// artifact.
    typedef std::string Version;
}

namespace __plugin_root::command {
    struct Command {
        Name         impl_name;
        CommandImpl  impl;
        CommandScope scope;
        StateErr     state;
    };

    /// @brief A map of commands registered by the plugin
    /// and the managed system.
    typedef std::unordered_map<Name, Command> Registry;
}

namespace __plugin_root::artifact {
    /// @brief A box of artifact attributes.
    typedef std::unique_ptr<ArtifactBody> Artifact;
}

namespace __plugin_root {
    struct Context {
        artifact::Artifact artifact;
    };

    /// @brief  Install the plugin module from the given
    /// artifact.
    /// @param  source artifact to install from/into.
    /// @return The artifact after installation.
    Context Install(const Context&);
    /// @brief Release and destroy plugin module assets.
    /// @param  source artifact owning the module.
    /// @return The artifact after releasing assets.
    Context Release(const Context&);

    /// @brief State group the state belongs to.
    /// @param state
    /// @return State group name.
    const std::string StateGroup(const StateErr&);
    /// @brief Human-readable explanation of the error
    ///        state.
    /// @param state
    /// @return Human-readable message.
    const std::string StateMessage(const StateErr&);

    std::ostream& operator<<(std::ostream&, const StateErr&);

    /// @brief Alias for:
    /// 
    /// ```cpp
    /// std::expected<R, StateErr>
    /// ```
    /// 
    /// Used in callback communication between plugins and
    /// the manager.
    template <typename R>
    using State = std::expected<R, StateErr>;
    template <typename R, typename ...A>
    using StateFn = std::function<State<R>(A...)>;
}

namespace __plugin_root::artifact {
    struct ArtifactBody {
        command::Registry commands;
        Desc        description;
        Desc        description_long;
        Path        manifest;
        Name        name;
        ObjectEntry obj_entry;
        ObjectMode  obj_mode;
        ObjectRef   obj_ref;
        StateErr    state;
        Version     version;
    };

    std::ostream& operator<<(std::ostream&, const Artifact&);

    /// @brief  Create a new, empty plugin artifact.
    /// @return A new artifact.
    Artifact New(void);
    /// @brief Create a new plugin artifact from an object
    ///        path.
    /// @param  path path to object path.
    /// @return A new object.
    Artifact New(const ObjectEntry);
    /// @brief Create a new plugin artifact from a
    ///        manifest file.
    /// @param  path the manifest file path.
    /// @return A new artifact object.
    Artifact FromFile(const char *);
    /// @brief Create a copy of one artifact from another.
    /// @param  source Source artifact.
    /// @return A new object.
    Artifact FromOther(const Artifact&);
    /// @brief Create a copy of the source artifact with a
    ///        brief description of the plugin.
    /// @param  source Artifact to copy from.
    /// @param  desc Description value.
    /// @return A new object.
    Artifact WithDescription(const Artifact&, const Desc);
    /// @brief Create a copy of the source artifact with a
    ///        detailed description of the plugin.
    /// @param  source Artifact to copy from.
    /// @param  desc Description value.
    /// @return A new object.
    Artifact WithDescriptionLong(const Artifact&, const Desc);
    /// @brief Create a copy of the source artifact with a
    ///        specified object path.
    /// @param  source Artifact to copy from.
    /// @param  path Name of the file path where the
    ///         artifact should be loaded from.
    /// @return A new object.
    Artifact WithEntry(const Artifact&, const ObjectEntry);
    /// @brief Create a copy of the source artifact with a
    ///        path to the file containing its manifest.
    /// @param  source Artifact to copy from.
    /// @param  path to the artifact manifest.
    /// @return A new object.
    Artifact WithManifest(const Artifact&, const Path);
    /// @brief Create a copy of the source artifact with a
    ///        specified name.
    /// @param  source Artifact to copy from.
    /// @param  name Name to set on the artifact.
    /// @return A new object.
    Artifact WithName(const Artifact&, const Name);
    /// @brief Create a copy of the source artifact with a
    ///        specified state.
    /// @param  source Artifact to copy from.
    /// @param  state The state of the new artifact.
    /// @return A new object.
    Artifact WithState(const Artifact&, const StateErr);
    /// @brief  Create a copy of the source artifact with a
    ///         specified version.
    /// @param  source Artifact to copy from.
    /// @param  version The version of the new artifact.
    /// @return A new object.
    Artifact WithVersion(const Artifact&, const Version);
}

namespace __plugin_root::command {
    Command LoadFromArtifact(const artifact::Artifact&, const Name, const Name);
}

namespace __plugin_root::plugin {
    /// @brief Can add/register commands to the global
    /// system.
    class HasCommands {
        /// @brief  Allows a plugin to register a command
        ///         with the system.
        /// @param  context Context passed from the manager.
        /// @return The success state of this hook when
        ///         called as the `context`.
        virtual State<Context> AddCommand(const Context) = 0;
        /// @brief  Calls a command from the context.
        /// @param  context Context passed from the manager.
        /// @param  name Name of the command being called.
        /// @return The success state of the command called.
        virtual State<Context> CallCommand(const Context, Name) = 0;
    };

    /// @brief Can install as a plugin.
    class CanInstall {
        /// @brief  Callback hook for when the plugin is
        ///         installed and loaded.
        /// @param  context Context passed from the manager.
        /// @return The success state of this hook when
        ///         called as the `context`.
        virtual State<Context> OnInstall(const Context) = 0;
    };

    /// @brief Can release plugin assets.
    class CanRelease {
        /// @brief  Callback hook for when the plugin is
        ///         released.
        /// @param  context Context passed from the manager.
        /// @return The success state of this hook when
        ///         called as the `context`.
        virtual State<Context> OnRelease(const Context) = 0;
    };

    /// @brief Entry point for an external system, code or
    /// behavior to interact with the host system.
    class Plugin {
        public:
        virtual ~Plugin(void) = default;
        Plugin(const Context);
    };
}
