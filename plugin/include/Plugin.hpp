/// @file plugin.hpp
/// @brief Main plugin API declarations. Defines type
/// aliases, traits and functions that are publicly
/// available to the CXX interface.

#pragma once

#include <expected>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <unordered_map>

#include "plugin.h"

// Has to be here as a forward ref of `Artifact` as this is
// a first-class type.
namespace __plugin_root::artifact {
    /// @brief A box of artifact attributes.
    typedef std::unique_ptr<ArtifactBody> Artifact;
}

namespace __plugin_root {
    /// @brief The description of a plugin asset, module or
    /// artifact.
    typedef std::string Desc;
    /// @brief The name of a plugin asset, module or
    /// artifact.
    typedef std::string Name;
    /// @brief Path to the object entry point.
    typedef std::string ObjectEntry;
    /// @brief Mode of how to build the entry point.
    typedef std::string ObjectMode;
    /// @brief A reference to the object loaded dynamically.
    typedef void *ObjectRef;
    /// @brief Type of the entry point.
    typedef std::string ObjectType;
    /// @brief Path to an arbitrary file or directory.
    typedef std::string Path;
    /// @brief A message providing more detail about the
    /// `StateErr` which occured.
    typedef std::string StateErrMessage;
    /// @brief The version name of a plugin asset, module or
    /// artifact.
    typedef std::string Version;

    /// @brief The grouping some `StateErr` belongs to.
    typedef enum StateErrGroup {
        OK,
        DYNAMIC,
        MANIFEST,
        USER, // Namespace indicates all user-defined errors.
    } StateErrGroup;

    const std::map<StateErrGroup, StateErrMessage> StateErrGroupNames = {
        { OK,       "OK"  },
        { DYNAMIC,  "DLL" },
        { MANIFEST, "MAN" },
        { USER,     "USR" }
    };

    /// @brief The system-defined error messages designated
    /// to each kind of `StateErr`.
    const std::map<StateErr, StateErrMessage> StateErrMessages = {
        { NONE,           "no error"                       },
        { DLL_MALFORMAT,  "bad dll format"                 },
        { DLL_NOCALLBACK, "missing callback"               },
        { DLL_NOINSTALL,  "artifact not installed"         },
        { DLL_NOMODULE,   "missing module"                 },
        { DLL_NOOBJECT,   "missing module object"          },
        { DLL_NORELEASE,  "artifact not released"          },
        { DLL_NOSCOPE,    "missing scope of module object" },
        { MAN_NOHEADER,   "missing manifest header"        },
        { MAN_NOMAN,      "missing manifest file"          },
        { MAN_MALPARSE,   "bad format in manifest"         },
    };

    /// @brief Provides information given the current state
    /// in detail.
    typedef struct StateInfo {
        StateErr        code;
        StateErrMessage detail;
        StateErrGroup   group;
        StateErrMessage message;

        StateInfo(void);
        StateInfo(const StateErr&);
        StateInfo(const StateErr&, const StateErrMessage&);
    } StateInfo;

    struct Context {
        artifact::Artifact artifact;
    };

    /// @brief  Install the plugin module from the given
    /// artifact manifest.
    /// @param path of the artifact owning the module.
    /// @return context passed to this function.
    Context Install(const char *);
    /// @brief  Install the plugin module from the given
    /// artifact.
    /// @param ctx of the artifact owning the module.
    /// @return context passed to this function.
    Context Install(const Context&);
    /// @brief Initilizes the plugin module. This is
    /// effectively the 'start-up' functionality.
    /// @param ctx of the artifact owning the module.
    /// @return context passed to this function.
    Context Initialize(const Context&);
    /// @brief Release and destroy plugin module assets.
    /// @param ctx of the artifact owning the module.
    /// @return context passed to this function.
    Context Release(const Context&);

    /// @brief State group the state belongs to.
    /// @param state
    /// @return State group name.
    template <typename T, typename V>
    const T StateGroup(const V&);
    template <>
    const std::string StateGroup(const StateErr&);
    template <>
    const std::string StateGroup(const StateErrGroup&);
    template <>
    const StateErrGroup StateGroup(const StateErr&);
    /// @brief Human-readable explanation of the error
    ///        state.
    /// @param state
    /// @return Human-readable message.
    const std::string StateMessage(const StateErr&);

    // Overloading operators to create communicabilitly
    // between both objects, as they are effectively the
    // same. But where one provides more detail than the
    // other, we need to be able to morph between the two.
    StateInfo operator*(StateErr);
    StateInfo operator&(StateErr, StateErrMessage);
    std::ostream& operator<<(std::ostream&, const StateErr&);
    StateErr operator*(StateInfo);
    bool operator==(StateInfo, StateErr);
    std::ostream& operator<<(std::ostream&, const StateInfo&);

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

namespace __plugin_root::command {
    struct Command {
        Name            impl_name;
        CommandImpl     impl;
        CommandScope    scope;
        StateInfo       state;
    };

    /// @brief A map of commands registered by the plugin
    /// and the managed system.
    typedef std::unordered_map<Name, Command> Registry;
}

namespace __plugin_root::artifact {
    struct ArtifactBody {
        command::Registry commands;
        Desc            description;
        Desc            description_long;
        Path            manifest;
        Name            name;
        ObjectEntry     obj_entry;
        ObjectMode      obj_mode;
        ObjectRef       obj_ref;
        ObjectType      obj_type;
        StateInfo       state;
        Version         version;

        bool IsInstalled(void);
        void SetState(const StateInfo);
    };

    /// @brief Return a string representation of an
    /// `Artifact`.
    /// @param source artifact.
    /// @return string representation of the object.
    const std::string Repr(const Artifact&);
    /// @brief Return a string representation of an
    /// `Artifact`.
    /// @param source artifact.
    /// @return JSON string representation of the
    const std::string ReprJson(const Artifact&);
    /// @brief Return a string representation of an
    /// `Artifact`.
    /// @param source artifact.
    /// @return TOML string representation of the
    const std::string ReprToml(const Artifact&);
    /// @brief Return a string representation of an
    /// `Artifact`.
    /// @param source artifact.
    /// @return YAML string representation of the
    const std::string ReprYaml(const Artifact&);
    std::ostream& operator<<(std::ostream&, const Artifact&);

    /// @brief Get the name of the artifact.
    /// @param source to inspect.
    /// @return Artifact object name.
    const Name GetName(const Artifact&);
    /// @brief Get the description of the artifact.
    /// @param source to inspect.
    /// @return Artifact object description.
    const Desc GetDescription(const Artifact&);
    /// @brief Get the long description of the artifact.
    /// @param source to inspect.
    /// @return Artifact object long description.
    const Desc GetDescriptionLong(const Artifact&);
    /// @brief Get the module entry point of the artifact.
    /// @param source to inspect.
    /// @return Artifact module entry point.
    const ObjectEntry GetEntry(const Artifact&);
    /// @brief Get the module builder mode.
    /// @param source to inspect.
    /// @return Artifact builder mode.
    const ObjectMode GetMode(const Artifact&);
    /// @brief Get the module builder type.
    /// @param source to inspect.
    /// @return Artifact builder type.
    const ObjectType GetType(const Artifact&);

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
    /// brief description of the plugin.
    /// @param source Artifact to copy from.
    /// @param desc Description value.
    /// @return A new object.
    Artifact WithDescription(const Artifact&, const Desc);
    /// @brief Create a copy of the source artifact with a
    /// detailed description of the plugin.
    /// @param source Artifact to copy from.
    /// @param desc Description value.
    /// @return A new object.
    Artifact WithDescriptionLong(const Artifact&, const Desc);
    /// @brief Create a copy of the source artifact with a
    /// specified object path.
    /// @param source Artifact to copy from.
    /// @param path Name of the file path where the
    /// artifact should be loaded from.
    /// @return A new object.
    Artifact WithEntry(const Artifact&, const ObjectEntry);
    /// @brief Create a copy of the source artifact with a
    /// path to the file containing its manifest.
    /// @param source Artifact to copy from.
    /// @param path to the artifact manifest.
    /// @return A new object.
    Artifact WithManifest(const Artifact&, const Path);
    /// @brief Create a copy of the source artifact with a
    /// mode of how the artifact is supposed to be built for
    /// packaging.
    /// @param source Artifact to copy from.
    /// @param mode of how to build the artifact module.
    /// @return A new object.
    Artifact WithMode(const Artifact&, const ObjectMode);
    /// @brief Create a copy of the source artifact with a
    /// specified name.
    /// @param source Artifact to copy from.
    /// @param name Name to set on the artifact.
    /// @return A new object.
    Artifact WithName(const Artifact&, const Name);
    /// @brief Create a copy of the source artifact with a
    /// specified state.
    /// @param source Artifact to copy from.
    /// @param state The state of the new artifact.
    /// @return A new object.
    Artifact WithState(const Artifact&, const StateErr);
    /// @brief Create a copy of the source artifact with a
    /// specified state.
    /// @param source Artifact to copy from.
    /// @param state The state of the new artifact.
    /// @param detail The message providing more context.
    /// @return A new object.
    Artifact WithState(const Artifact&, const StateErr, const StateErrMessage);
    /// @brief Create a copy of the source artifact with a
    /// specified module type.
    /// @param source Artifact to copy from.
    /// @param type The module type to use.
    /// @return A new object.
    Artifact WithType(const Artifact&, const ObjectType);
    /// @brief Create a copy of the source artifact with a
    /// specified version.
    /// @param source Artifact to copy from.
    /// @param version The version of the new artifact.
    /// @return A new object.
    Artifact WithVersion(const Artifact&, const Version);
}

namespace __plugin_root::command {
    typedef std::optional<command::CommandImpl> CommandImplDefault;
    /// @brief Does absolutely nothing.
    /// @param ctx context to pass to command.
    /// @return Should always return 'NONE' state.
    StateErr DefaultPassthrough(const Context *);
    Command LoadFromArtifact(const artifact::Artifact&, const Name, const Name);
    Command LoadFromArtifact(const artifact::Artifact&, const Name, const Name, CommandImplDefault);
}

/// @brief Logic and behavior dedicated to building a single
/// plugin.
namespace __plugin_root::package {
    /// @brief Build the module according to how its
    /// configuration dictates how it should be expected.
    /// @param source Artifact to reference from.
    /// @return The result of how the build went.
    StateErr Build(const artifact::Artifact&);
}
