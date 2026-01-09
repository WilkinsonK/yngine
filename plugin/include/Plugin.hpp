/// @file plugin.hpp
/// @brief Main plugin API declarations. Defines type
/// aliases, traits and functions that are publicly
/// available to the CXX interface.

#pragma once

#include <expected>
#include <filesystem>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include "plugin.h"

// Has to be here as a forward ref of `Artifact` as this is
// a first-class type.
namespace __plugin_root::artifact {
    /// @brief A box of artifact attributes.
    typedef std::unique_ptr<Body> Artifact;
}

namespace __plugin_root::package {
    typedef struct Body Body;
    typedef struct Entry Entry;
    typedef std::unique_ptr<Body> Package;
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
    /// @brief A reference to the object loaded dynamically.
    typedef void *ObjectRef;
    /// @brief Path to an arbitrary file or directory.
    typedef std::string Path;
    /// @brief A message providing more detail about the
    /// `StateErr` which occured.
    typedef std::string StateErrMessage;
    /// @brief The version name of a plugin asset, module or
    /// artifact.
    typedef std::string Version;

    /// @brief Mode of how to build the entry point.
    typedef enum BuildMode {
        BM_NONE,
        BM_CMAKE,
        BM_CUSTOM,
        BM_MAKE,
        BM_XMAKE
    } BuildMode;

    const std::map<std::string, BuildMode> BuildModes = {
        { "none",   BuildMode::BM_NONE   },
        { "cmake",  BuildMode::BM_CMAKE  },
        { "custom", BuildMode::BM_CUSTOM },
        { "make",   BuildMode::BM_MAKE   },
        { "xmake",  BuildMode::BM_XMAKE  }
    };

    /// @brief Type of the entry point.
    typedef enum BuildType {
        BT_NONE,
        BT_DYNAMIC,
        BT_NODE,
        BT_PY3
    } BuildType;

    const std::map<std::string, BuildType> BuildTypes = {
        { "none",    BuildType::BT_NONE    },
        { "dynamic", BuildType::BT_DYNAMIC },
        { "node",    BuildType::BT_NODE    },
        { "py3",     BuildType::BT_PY3     }
    };

    /// @brief The grouping some `StateErr` belongs to.
    typedef enum StateErrGroup {
        OK,
        DYNAMIC,
        MANIFEST,
        BUILD,
        USER, // Namespace indicates all user-defined errors.
    } StateErrGroup;

    const std::map<StateErrGroup, StateErrMessage> StateErrGroupNames = {
        { OK,       "OK"  },
        { DYNAMIC,  "DLL" },
        { MANIFEST, "MAN" },
        { BUILD,    "BLD" },
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
        { BLD_MODEUNK,    "unknown build mode"             },
        { BLD_MALPKG,     "could not build plugin package" },
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
        package::Package   package;

        static Context FromManifest(const std::filesystem::path);
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
        Name         impl_name;
        CommandImpl  impl;
        CommandScope scope;
        StateInfo    state;
    };

    /// @brief A map of commands registered by the plugin
    /// and the managed system.
    typedef std::unordered_map<Name, Command> Registry;
}

namespace __plugin_root::artifact {
    struct Body {
        command::Registry commands;
        Desc            description;
        Desc            description_long;
        Path            manifest;
        Name            name;
        ObjectEntry     obj_entry;
        BuildMode       obj_mode;
        ObjectRef       obj_ref;
        BuildType       obj_type;
        StateInfo       state;
        Version         version;

        bool IsInstalled(void);
        void SetState(const StateInfo);

        /// @brief Get the name of the artifact.
        /// @return Artifact object name.
        const Name GetName(void);
        /// @brief Get the description of the artifact.
        /// @return Artifact object description.
        const Desc GetDescription(void);
        /// @brief Get the long description of the artifact.
        /// @return Artifact object long description.
        const Desc GetDescriptionLong(void);
        /// @brief Get the module entry point of the artifact.
        /// @return Artifact module entry point.
        const ObjectEntry GetEntry(void);
        /// @brief Get the module builder mode.
        /// @return Artifact builder mode.
        const BuildMode GetMode(void);
        /// @brief Get the module builder type.
        /// @return Artifact builder type.
        const BuildType GetType(void);

        /// @brief Return a string representation of an
        /// `Artifact`.
        /// @return string representation of the object.
        const std::string Repr(void);
        /// @brief Return a string representation of an
        /// `Artifact`.
        /// @return JSON string representation of the
        const std::string ReprJson(void);
        /// @brief Return a string representation of an
        /// `Artifact`.
        /// @return TOML string representation of the
        const std::string ReprToml(void);
        /// @brief Return a string representation of an
        /// `Artifact`.
        /// @return YAML string representation of the
        const std::string ReprYaml(void);

        struct Builder {
            Artifact inner;

            Builder(void);
            Builder(std::filesystem::path);
            Builder(const Artifact&);
            Artifact Build(void);

            /// @brief Create a copy of the source artifact with a
            /// brief description of the plugin.
            /// @param desc Description value.
            /// @return A new object.
            Builder& WithDescription(const Desc);
            /// @brief Create a copy of the source artifact with a
            /// detailed description of the plugin.
            /// @param desc Description value.
            /// @return A new object.
            Builder& WithDescriptionLong(const Desc);
            /// @brief Create a copy of the source artifact with a
            /// specified object path.
            /// @param path Name of the file path where the
            /// artifact should be loaded from.
            /// @return A new object.
            Builder& WithEntry(const ObjectEntry);
            /// @brief Create a copy of the source artifact with a
            /// path to the file containing its manifest.
            /// @param path to the artifact manifest.
            /// @return A new object.
            Builder& WithManifest(const Path);
            /// @brief Create a copy of the source artifact with a
            /// specified build mode.
            /// @param mode artifact is to be built with.
            /// @return A new object.
            Builder& WithMode(const std::string);
            /// @brief Create a copy of the source artifact with a
            /// specified build mode.
            /// @param mode artifact is to be built with.
            /// @return A new object.
            Builder& WithMode(const BuildMode);
            /// @brief Create a copy of the source artifact with a
            /// specified name.
            /// @param name Name to set on the artifact.
            /// @return A new object.
            Builder& WithName(const Name);
            /// @brief Create a copy of the source artifact with a
            /// specified state.
            /// @param source Artifact to copy from.
            /// @param state The state of the new artifact.
            /// @return A new object.
            Builder& WithState(const StateErr);
            /// @brief Create a copy of the source artifact with a
            /// specified state.
            /// @param source Artifact to copy from.
            /// @param state The state of the new artifact.
            /// @param detail The message providing more context.
            /// @return A new object.
            Builder& WithState(const StateErr, const StateErrMessage);
            /// @brief Create a copy of the source artifact with a
            /// specified module type.
            /// @param source Artifact to copy from.
            /// @param type The module type to use.
            /// @return A new object.
            Builder& WithType(const BuildType);
            /// @brief Create a copy of the source artifact with a
            /// specified version.
            /// @param source Artifact to copy from.
            /// @param version The version of the new artifact.
            /// @return A new object.
            Builder& WithVersion(const Version);
        };
    };

    using Builder = Body::Builder;

    std::ostream& operator<<(std::ostream&, const Artifact&);
    std::ostream& operator<<(std::ostream&, const Body&);

    /// @brief  Create a new, empty plugin artifact.
    /// @return A new artifact.
    Artifact New(void);
    /// @brief Create a new plugin artifact from an object
    /// path.
    /// @param  path path to object path.
    /// @return A new object.
    Artifact New(const ObjectEntry);
    /// @brief Create a new plugin artifact from a
    /// manifest file.
    /// @param  path the manifest file path.
    /// @return A new artifact object.
    Artifact FromManifest(const std::filesystem::path);
    /// @brief Create a copy of one artifact from another.
    /// @param  source Source artifact.
    /// @return A new object.
    Artifact FromOther(const Artifact&);
    /// @brief Create a copy of one artifact from another.
    /// @param  source Source artifact.
    /// @return A new object.
    Artifact FromOther(const Body&);
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
    struct Body {
        std::vector<Entry>                   included_files;
        std::string                          manifest;
        std::string                          name;
        std::optional<std::filesystem::path> output_dir;
        std::optional<std::filesystem::path> source_dir;
        std::string                          version;

        static Package New(void);
        static Package FromManifest(std::filesystem::path);

        std::filesystem::path GetManifest(void);
        std::filesystem::path GetOutputDir(void);
        std::filesystem::path GetOutputName(void);
        std::filesystem::path GetSourceDir(void);

        struct Builder {
            Package inner;

            Builder(void);
            Builder(std::filesystem::path);
            Package Build(void);
            Builder& AddInclude(Entry);
            Builder& WithManifestName(std::string);
            Builder& WithName(std::string);
            Builder& WithOutputDir(std::filesystem::path);
            Builder& WithSourceDir(std::filesystem::path);
            Builder& WithVersion(std::string);
        };
    };

    using Builder = Body::Builder;

    struct Entry {
        /// @brief Alias name used when the file is written
        /// to package archive.
        Name alias;
        /// @brief Source path of the file to archive from.
        Path source;
    };

    /// @brief Build the module according to how its
    /// configuration dictates how it should be expected.
    /// @param source Artifact to reference from.
    /// @return The result of how the build went.
    StateInfo Build(const Context&);
    /// @brief Create a plugin package from an artifact.
    /// @param source Artifact to reference from.
    /// @return The result of how packaging went.
    StateInfo Pack(const Context&);
    StateInfo Unpack(const Context&);

    Package New(void);
    Package FromManifest(std::filesystem::path);
}
