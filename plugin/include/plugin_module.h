/// @file plugin_module.h
/// @brief Defines utility macros for declaring some source
/// as part of a plugin module.

#include "plugin.h"

#ifdef __cplusplus
/// @brief The header/preamble required for declarations
/// that must be available to the plugin ecosystem.
#define plugin_module_head \
    using namespace plugin; \
    using namespace plugin::command;
/// @brief The footer/closer required for declarations that
/// must be available to the plugin ecosystem.
#define plugin_module_tail
/// @brief Exports the object/callable/etc. without mangling
/// the names, making it available at load time.
#define plugin_module_export extern "C"
#else
/// @brief The header/preamble required for declarations
/// that must be available to the plugin ecosystem.
#define plugin_module_head
/// @brief The footer/closer required for declarations that
/// must be available to the plugin ecosystem.
#define plugin_module_tail
/// @brief Exports the object/callable/etc. without mangling
/// the names, making it available at load time.
#define plugin_module_export
#endif

/// @brief Alias for `plugin_module_export` for convenience.
/// Depending on the language of the module (c/c++),
/// includes implicit extern calls to prevent name mangling
/// when the attribute needs to be accessed.
#define attribute plugin_module_export

/// @brief Declares the initial definition of a callback.
#define callback_def StateErr
/// @brief Declares the identity of a callback.
/// @param name Base identity of the callback.
#define callback_ident(name, ...) name##_callback##__VA_OPT__(__VA_ARGS__)
/// @brief Declares the callback signagure used by all
/// callback declarations.
#define callback_sig (const ContextBody *context)
/// @brief Declares the signature of a callback.
/// @param name Base identity of the callback.
#define callback_decl(name) plugin_module_export callback_def callback_ident(name) callback_sig
/// @brief Declares a raw callback. A raw callback is a
/// series of artifacts which, when composed, define a
/// `Command` within the plugin ecosystem.
/// @param name The internal name of the callback being
/// defined.
/// @param ident The identity of callback attributes. Used
/// to identify `scope` and `implementation` members.
/// @param scope The scope of availability this callback is
/// available to the system.
#define callback_raw(name, ident, scope) \
    callback_decl(name); \
    attribute const CommandScope callback_ident(ident, Scope) = scope; \
    attribute const CommandImpl  callback_ident(ident, Impl)  = callback_ident(name); \
    callback_decl(name)
/// @brief Declares a user-defined scoped callback.
/// @param name Base identity of the callback.
/// @param scope The scope of availability this callback is
/// available to the system.
#define callback(name, scope) callback_raw(name, name, scope)
/// @brief Declares the install callback. This callback will
/// be loaded and used to install the plugin.
/// @param name Base identity of the installer callback.
#define oninstall(name) callback_raw(name, OnInstall, SCOPE_GLOBAL)
/// @brief Declares the release callback. This callback will
/// be loaded and used to destroy the plugin and release its
/// assets (if necessary).
/// @param name Base identity of the releaser callback.
#define onrelease(name) callback_raw(name, OnRelease, SCOPE_GLOBAL)

/// @brief Encloses the wrapped declarations as exportable
/// items that are then made available to the plugin
/// ecosystem as a result.
/// 
/// ```c
/// #include "plugin_module.h"
/// 
/// plugin_module(
///     oninstall(Init) {}
///     onrelease(Drop) {}
///     ... // And whatever else needs to be exported.
/// )
/// ```
#define plugin_module(...) \
plugin_module_head \
__VA_ARGS__ \
plugin_module_tail
