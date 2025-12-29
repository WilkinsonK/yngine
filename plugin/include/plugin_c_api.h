#ifndef PLUGIN_C_API_H
#define PLUGIN_C_API_H
#include "plugin.h"

#ifdef __cplusplus
extern "C" {
#endif // extern "C"

#ifdef __cplusplus
typedef plugin::StateErr StateErr;
typedef plugin::Context Context;
#endif

typedef char *Desc;
typedef char *Name;

StateErr plugin_call_command(const Context *, const Name);
const char *plugin_get_description(const Context *);
const char *plugin_get_description_long(const Context *);
const char *plugin_get_name(const Context *);
StateErr plugin_register_command(const Context*, const Name, const Name);

#ifdef __cplusplus
}
#endif // extern "C"
#endif // PLUGIN_C_API_H
