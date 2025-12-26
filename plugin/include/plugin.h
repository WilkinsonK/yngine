/// @file plugin.h
/// @brief Common types and declarations shared between the
/// core declarations and module declarations.

#ifndef PLUGIN_PLUGIN_H
#define PLUGIN_PLUGIN_H

#define __plugin_root plugin

#ifdef __cplusplus
#define __ns_cplusplus_head(name) namespace name {
#define __ns_cplusplus_tail }
#define __ns_cplusplus_prop(ns, name) ns::name
#else
#define __ns_cplusplus_head(name)
#define __ns_cplusplus_tail
#define __ns_cplusplus_prop(ns, name) name
#endif

__ns_cplusplus_head(__plugin_root)
    /// @brief Context data specific to a plugin. Is
    /// typically passed between the manager and the
    /// instance.
    typedef struct Context Context;

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
__ns_cplusplus_tail

__ns_cplusplus_head(__plugin_root::artifact)
    /// @brief Wraps all assets related to a single plugin.
    typedef struct ArtifactBody ArtifactBody;
__ns_cplusplus_tail

__ns_cplusplus_head(__plugin_root::command)
    /// @brief The callable and attributes dictating its
    /// behavior.
    typedef struct Command Command;
    /// @brief Callable hook that is arbitrary to the
    /// manager and defined by the plugin.
    typedef StateErr(*CommandImpl)(const Context*);
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
__ns_cplusplus_tail

__ns_cplusplus_head(__plugin_root)
/// @brief Calls a command from the registry available
/// within this context.
/// @param  ctx as the current plugin context.
/// @param  name as the identifier used in the registry.
/// @return The state of whether the call was successful.
StateErr CallCommand(const Context *, const char *);
/// @brief Gets the name of the artifact currently contained
/// in the context.
/// @param  ctx as the current context.
/// @return The name of the artifact.
const char *GetName(const Context *);
/// @brief Gets the description of the artifact currently
/// contatained in the context.
/// @param  ctx as the current context.
/// @return The description of the artifact.
const char *GetDescription(const Context *);
/// @brief Gets the long description of the artifact
/// currently contatained in the context.
/// @param  ctx as the current context.
/// @return The long description of the artifact.
const char *GetDescriptionLong(const Context *);
/// @brief Adds a command to the artifact registry; making
/// it callable from the plugin ecosystem.
/// @param  ctx as the current plugin context.
/// @param  name as the identifier used in the registry.
/// @param  impl_name as the identifier used to locate the
/// implementation.
/// @return The state of whether registration was
/// successful.
StateErr RegisterCommand(const Context *, const char *, const char *);
__ns_cplusplus_tail

#endif // #ifndef PLUGIN_PLUGIN_H
