/// @file plugin.hpp
/// @brief Main plugin API declarations. Defines type
/// aliases, traits and functions that are publicly
/// available to the CXX interface.

#pragma once

#include <expected>
#include <iostream>
#include <memory>
#include <unordered_map>

#include "plugin.h"

namespace plugin {
    /// @brief The description of a plugin asset, module or
    /// artifact.
    typedef std::string Desc;
    /// @brief The name of a plugin asset, module or
    /// artifact.
    typedef std::string Name;
    /// @brief Path to the object binary.
    typedef std::string ObjectPath;
    /// @brief Mode of how to load the binary.
    typedef int ObjectMode;
    /// @brief A reference to the object loaded dynamically.
    typedef void *ObjectRef;
    /// @brief Path to an arbitrary file or directory.
    typedef std::string Path;
    /// @brief The version name of a plugin asset, module or
    /// artifact.
    typedef std::string Version;
}

namespace plugin::command {
    struct Command {
        CommandImpl  impl;
        CommandScope scope;
        StateErr     state;
    };

    /// @brief A map of commands registered by the plugin
    /// and the managed system.
    typedef std::unordered_map<Name, Command> Registry;
}

namespace plugin::artifact {
    struct ArtifactBody {
        command::Registry commands;
        Desc        description;
        Desc        description_long;
        Path        manifest;
        Name        name;
        ObjectMode  obj_mode;
        ObjectPath  obj_path;
        ObjectRef   obj_ref;
        StateErr    state;
        Version     version;
    };

    /// @brief A box of artifact attributes.
    typedef std::unique_ptr<ArtifactBody> Artifact;
}

namespace plugin {
    struct Context {
        artifact::Artifact artifact;
    };

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
}

namespace plugin::artifact {
    std::ostream& operator<<(std::ostream&, const Artifact&);

    Artifact Install(const Artifact&);
    Artifact Release(const Artifact&);

    /// @brief  Create a new, empty plugin artifact.
    /// @return A new artifact.
    Artifact New(void);
    /// @brief Create a new plugin artifact from an object
    ///        path.
    /// @param  path path to object path.
    /// @return A new object.
    Artifact New(const ObjectPath);
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
    ///        specified object path.
    /// @param  source Artifact to copy from.
    /// @param  path Name of the file path where the
    ///         artifact should be loaded from.
    /// @return A new object.
    Artifact WithPath(const Artifact&, const ObjectPath);
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

namespace plugin::command {
    Command LoadFromArtifact(const artifact::Artifact&, const Name);
}

namespace plugin::plugin {
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
