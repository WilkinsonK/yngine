#pragma once

#include <expected>

#include "Plugin.hpp"

namespace plugin {
    typedef void* DllHandle;
    typedef const char *EndpointName;
    typedef std::function<DllHandle(void)> DllLoader;

    #define HandleResult PluginResult(plugin::DllHandle)
    PluginResult(DllHandle) DllFileAvailable(PluginArtifact&);
    PluginResult(DllHandle) DllLoad(DllLoader);
    PluginResult(DllHandle) DllLoadEndpoint(PluginArtifact&, DllHandle, EndpointName);
    PluginResult(DllHandle) DllLoad_DropPlugin(PluginArtifact&, DllHandle);
    PluginResult(DllHandle) DllLoad_MakePlugin(PluginArtifact&, DllHandle);
    PluginResult(DllHandle) DllLoadLibrary(PluginArtifact& artifact);
}
