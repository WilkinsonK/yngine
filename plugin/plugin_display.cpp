#include <iostream>
#include <sstream>
#include <string>

#include "Plugin.hpp"

namespace plugin {
    void RenderArtifactElements(std::ostream& os, const PluginArtifacts& artifacts, size_t i) {
        os << i << ": " << *artifacts[i];
        if (i == artifacts.size()-1) return;
        os << "," ;
        RenderArtifactElements(os, artifacts, ++i);
    }

    std::ostream& RenderArtifactElements(std::ostream& os, const PluginArtifacts& artifacts) {
        RenderArtifactElements(os, artifacts, 0);
        return os;
    }

    const std::string RenderArtifactElements(const PluginArtifacts& artifacts) {
        std::stringstream ss;
        RenderArtifactElements(ss, artifacts);
        return ss.str();
    }

    std::ostream& operator<<(std::ostream& os, const PluginArtifact_ a) {
        return os << "PluginArtifact(mem='" << &a << "', status='" << a.status << "'";
    }

    std::ostream& operator<<(std::ostream& os, const PluginArtifact a) {
        return os << *a;
    }

    std::ostream& operator<<(std::ostream& os, const PluginArtifacts& artifacts) {
        return os << "Artifacts[" << RenderArtifactElements(artifacts) << "]";
    }

    std::ostream& operator<<(std::ostream& os, const PluginManager& p) {
        os << "PluginManager(" ;
        os <<  "mem='" << &p << "'," "artifacts=" << p.GetArtifacts() <<  ")";
        return os;
    }
}
