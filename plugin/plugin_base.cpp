#include <exception>
#include <expected>
#include <iostream>
#include <memory>

#include "Plugin.hpp"

using namespace plugin;

PluginBase::PluginBase(const Context_ c) : Plugin(c), context(c) {}

PluginResult(int) PluginBase::OnLoad(Context global) {
    return 0;
}

void PluginBase::OnRelease(Context) {
    return;
}

PluginResult(PluginName) PluginBase::GetName(void) {
    return context.plugin_name;
}

PluginResult(PluginVersion) PluginBase::GetVersion(void) {
    return context.plugin_version;
}
