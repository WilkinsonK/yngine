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
// Alias CXX functions to match C-ABI.
#define plugin_call_command(cmd_name) plugin::CallCommand(ctx, cmd_name)
#define plugin_get_description() plugin::GetDescription(ctx)
#define plugin_get_description_long() plugin::GetDescriptionLong(ctx)
#define plugin_get_name() plugin::GetName(ctx)
#define plugin_register_command(cmd_name, cmd_impl_name) plugin::RegisterCommand(ctx, cmd_name, cmd_impl_name)
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
// Redefines the API calls to wrap around `Context`
// instances.
#define plugin_call_command(cmd_name) plugin_call_command(ctx, cmd_name)
#define plugin_get_description() plugin_get_description(ctx)
#define plugin_get_description_long() plugin_get_description_long(ctx)
#define plugin_get_name() plugin_get_name(ctx)
#define plugin_register_command(cmd_name, cmd_impl_name) plugin_register_command(ctx, cmd_name, cmd_impl_name)
#endif

/// @brief Alias for `plugin_module_export` for convenience.
/// Depending on the language of the module (c/c++),
/// includes implicit extern calls to prevent name mangling
/// when the attribute needs to be accessed.
#define attribute plugin_module_export

/// @brief Declares the initial definition of a command.
#define command_def StateErr
/// @brief Declares the identity of a command.
/// @param name Base identity of the command.
#define command_ident(name, ...) name##_command##__VA_OPT__(__VA_ARGS__)
/// @brief Declares the command signagure used by all
/// command declarations.
#define command_sig (const Context *ctx)
/// @brief Declares the signature of a command.
/// @param name Base identity of the command.
#define command_decl(name) plugin_module_export command_def command_ident(name) command_sig
/// @brief Declares a raw command. A raw command is a
/// series of artifacts which, when composed, define a
/// `Command` within the plugin ecosystem.
/// @param name The internal name of the command being
/// defined.
/// @param ident The identity of command attributes. Used
/// to identify `scope` and `implementation` members.
/// @param scope The scope of availability this command is
/// available to the system.
#define command_raw(name, ident, scope) \
    command_decl(name); \
    attribute const CommandScope command_ident(ident, Scope) = scope; \
    attribute const CommandImpl  command_ident(ident, Impl)  = command_ident(name); \
    command_decl(name)
/// @brief Declares a user-defined scoped command.
/// @param name Base identity of the command.
/// @param scope The scope of availability this command is
/// available to the system.
#define command(name, scope) command_raw(name, name, scope)
#define oninitialize(name) command_raw(name, OnInitialize, SCOPE_LOCAL)
/// @brief Declares the install callback. This callback will
/// be loaded and used to install the plugin.
/// @param name Base identity of the installer callback.
#define oninstall(name) command_raw(name, OnInstall, SCOPE_LOCAL)
/// @brief Declares the release callback. This callback will
/// be loaded and used to destroy the plugin and release its
/// assets (if necessary).
/// @param name Base identity of the releaser callback.
#define onrelease(name) command_raw(name, OnRelease, SCOPE_LOCAL)

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
