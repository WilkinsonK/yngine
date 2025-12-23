#include <iostream>
#include <expected>
#include <sstream>
#include <dlfcn.h>

#include "Plugin.hpp"

using namespace plugin;

PluginManager::PluginManager(void) : artifacts(), artifact_loader() {}

PluginResult(int) PluginManager::RegisterArtifact(PluginName name, PluginPath location) {
    artifacts.push_back(std::make_unique<PluginArtifact_>((PluginArtifact_){
        .name = name, .so_name = location
    }));
    return 0;
}

const PluginArtifacts& PluginManager::GetArtifacts(void) const {
    return artifacts;
}

PluginResult(int) PluginManager::LoadArtifacts(void) {
    PluginName so_name;

    for (std::size_t i = 0; i < artifacts.size(); i++)
        artifact_loader->Load(artifacts[i])
            .transform([&](int data){
                auto name = artifacts[i]->plugin->GetName();
                auto vers = artifacts[i]->plugin->GetVersion();
                std::cout << "Artifact loaded: " << name.value() << "-" << vers.value() << std::endl;
                return data;
            }).or_else([&](const std::string& err) {
                std::cerr << "Could not load plugin at '" << so_name << "'" << std::endl;
                std::cerr << "'" << *artifacts[i]->status << "'";
                return PluginResult(int)(0);
            });
        std::cout << *this << std::endl;
    return 0;
}

void PluginManager::ReleaseArtifacts(void) {
    for (std::size_t i = 0; i < artifacts.size(); i++) {
        auto name = std::string(artifacts[i]->plugin->GetName().value());
        auto vers = std::string(artifacts[i]->plugin->GetVersion().value());
        artifacts[i]->drop_plugin(artifacts[i]->plugin);
        artifacts[i]->drop_plugin = nullptr;
        artifacts[i]->make_plugin = nullptr;
        dlclose((void *)(artifacts[i]->so_data));
        std::cout << "Artifact released: " << name << "-" << vers << std::endl;
    }
}
