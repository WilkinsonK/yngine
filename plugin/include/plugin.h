/// @file plugin.h
/// @brief Common types and declarations shared between the
/// core declarations and module declarations.

#ifndef PLUGIN_PLUGIN_H
#define PLUGIN_PLUGIN_H

#ifdef __cplusplus
#define namespace_when_cpp_head(name) namespace name {
#define namespace_when_cpp_tail }
#else
#define namespace_when_cpp_head(name)
#define namespace_when_cpp_tail
#endif

#define namespace_when_cpp(name, ...) \
    namespace_when_cpp_head(name) \
    __VA_ARGS__ \
    namespace_when_cpp_tail

namespace_when_cpp(plugin::artifact,
    /// @brief Wraps all assets related to a single plugin.
    typedef struct ArtifactBody ArtifactBody;
)

namespace_when_cpp(plugin,
    /// @brief Context data specific to a plugin. Is
    /// typically passed between the manager and the
    /// instance.
    typedef struct ContextBody ContextBody;

    /// @brief Possible errors that can occur during the
    /// plugin lifecycle.
    typedef enum StateErr {
        NONE,
        /// @brief Object file is not in a format recognized
        /// by the host system.
        DLL_MALFORMAT = 0x1000,
        /// @brief Missing a callback/function from the
        /// object file.
        DLL_NOCALLBACK,
        /// @brief Missing entrypoint for destroying the
        /// plugin.
        DLL_NODROP,
        /// @brief Missing entrypoint for initializing the
        /// plugin.
        DLL_NOINIT,
        /// @brief Plugin/artifact was not installed.
        DLL_NOINSTALL,
        /// @brief No module could be found on path or it is
        /// unavailable.
        DLL_NOMODULE,
        /// @brief Missing an object/asset from the object
        /// file.
        DLL_NOOBJECT,
        /// @brief Plugin/artifact was not released.
        DLL_NORELEASE,
        /// @brief Missing scope attribute from the object
        /// file.
        DLL_NOSCOPE,
        /// @brief Missing manifest file.
        MAN_NOMAN = 0x2000,
        /// @brief Issue occured when parsing a manifest.
        MAN_MALPARSE,
        /// @brief The 'plugin' header was missing.
        MAN_NOPLUGIN,
    } StateErr;
)

namespace_when_cpp(plugin::command,
    /// @brief The callable and attributes dictating its
    /// behavior.
    typedef struct Command Command;
    /// @brief Callable hook that is arbitrary to the
    /// manager and defined by the plugin.
    typedef StateErr(*CommandImpl)(const ContextBody*);
    /// @brief The scope of the command and how/where it
    /// can be called from.
    typedef enum CommandScope {
        /// @brief Only visible to this plugin.
        SCOPE_LOCAL,
        /// @brief Visible to entire plugin system.
        SCOPE_GLOBAL,
        /// @brief Visible to the plugin manager.
        SCOPE_SHARED,
    } CommandScope;
)

#endif // #ifndef PLUGIN_PLUGIN_H
