#include <expected>
#include <filesystem>
#include <functional>
#include <sstream>
#include <dlfcn.h>

#include "Plugin.hpp"
#include "PluginLoader.hpp"

HandleResult plugin::DllFileAvailable(PluginArtifact& artifact) {
    PluginFound found  = std::filesystem::exists(artifact->so_name);
    artifact->status   = found ? "Available" : "Unavailable";
    artifact->so_found = found;
    if (!found) return std::unexpected("Missing plugin library");
    return nullptr;
}

HandleResult plugin::DllLoad(DllLoader loader) {
    if (auto handle = loader(); handle != nullptr) return handle;
    return std::unexpected(dlerror());
}

HandleResult plugin::DllLoadEndpoint(PluginArtifact& artifact, DllHandle data, EndpointName base_name) {
    return DllLoad([&](){
        std::stringstream ss;
        ss << base_name << artifact->name;
        std::cout << "Loading endpoint '" << ss.str() << "'" << std::endl;
        return dlsym(data, ss.str().c_str());
    }).or_else([&artifact, data](const std::string& err) {
        artifact->status = ("dlerr: " + err).c_str();
        dlclose(data);
        return PluginResult(void*)(nullptr);
    });
}

HandleResult plugin::DllLoad_DropPlugin(PluginArtifact& artifact, DllHandle data) {
    return DllLoadEndpoint(artifact, data, "DropPlugin_")
        .transform([&artifact, data](void* handle){
            artifact->drop_plugin = (DropPlugin)(handle);
            return data;
        });
}


HandleResult plugin::DllLoad_MakePlugin(PluginArtifact& artifact, DllHandle data) {
    return DllLoadEndpoint(artifact, data, "MakePlugin_")
        .transform([&artifact, data](void* handle){
            artifact->make_plugin = (MakePlugin)(handle);
            return data;
        });
}

HandleResult plugin::DllLoadLibrary(PluginArtifact& artifact) {
    return DllLoad([&](){
        std::cout << "Loading "
            << "library '" << artifact->name << "' "
            << "from '" << artifact->so_name << "'"
        << std::endl;
        return dlopen(artifact->so_name, RTLD_LAZY);
    });
}

PluginResult(int) plugin::PluginArtifactLoader::Load(PluginArtifact& artifact) {
    #define __op_okdef [&artifact](void* data)
    return DllFileAvailable(artifact)
        .and_then(__op_okdef {
            return DllLoadLibrary(artifact);
        }).and_then(__op_okdef {
            artifact->status = "Opened";
            return PluginResult(void*)(data);
        }).and_then(__op_okdef {
            return DllLoad_DropPlugin(artifact, data);
        }).and_then(__op_okdef {
            return DllLoad_MakePlugin(artifact, data);
        }).transform(__op_okdef {
            artifact->so_data   = data;
            artifact->so_loaded = true;
            artifact->status    = "Loaded";
            return data;
        }).transform(__op_okdef {
            artifact->plugin = artifact->make_plugin();
            artifact->status = "Installed";
            return data;
        }).or_else([&artifact](const std::string& err){
            std::cerr << "err: " + err << std::endl;
            artifact->status = err.c_str();
            artifact->so_loaded = false;
            return PluginResult(void*)(nullptr);
        }).has_value() ? 0 : 1;
}
