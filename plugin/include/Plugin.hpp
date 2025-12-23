#pragma once

#include <expected>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

#define PLUGIN_DEF_CONTEXT(Name, Version) \
    static const Context_ Name##_context = { \
        .plugin_name      = #Name, \
        .plugin_version   = #Version, \
        .operation        = "INSTALL" \
    }
#define PLUGIN_DEF_DROP(Name) \
    void DropPlugin_##Name(Plugin* p) { \
        delete p; \
    }
#define PLUGIN_DEF_INTER(Name) \
    PLUGIN_DEF_MAKE(Name) \
    PLUGIN_DEF_DROP(Name)
#define PLUGIN_DEF_MAKE(Name) \
    Plugin* MakePlugin_##Name(void) { \
        return new Name(Name##_context); \
    }
#define PLUGIN_PREAMBLE(Name, Version) \
    extern "C" { \
        PLUGIN_DEF_CONTEXT(Name, Version); \
        PLUGIN_DEF_INTER(Name) \
    }

#define PluginResult(R) std::expected<R, std::string>
#define Shared(T) std::shared_ptr<T>
#define Unique(T) std::unique_ptr<T>

namespace plugin {
    typedef const char *PluginName;
    #define PluginResult(R) std::expected<R, std::string>
    #define Shared(T) std::shared_ptr<T>
    #define Unique(T) std::unique_ptr<T>

    typedef const char *PluginVersion;
    typedef struct Context_ {
        PluginName    plugin_name;
        PluginVersion plugin_version;
        const char    *operation;
    } Context_;
    typedef Shared(Context_) Context;

    class Plugin {
        public:
        virtual ~Plugin(void) = default;
        Plugin(const Context_) {};
        virtual PluginResult(PluginName) GetName(void) = 0;
        virtual PluginResult(PluginVersion) GetVersion(void) = 0;
        virtual PluginResult(int) OnLoad(Context) = 0;
        virtual void OnRelease(Context) = 0;
    };

    class PluginBase : public Plugin {
        public:
        PluginBase(const Context_);
        PluginResult(PluginName) GetName(void);
        PluginResult(PluginVersion) GetVersion(void);
        PluginResult(int) OnLoad(Context);
        void OnRelease(Context);
        protected:
        const Context_ context;
    };

    typedef Plugin*(*MakePlugin)(void);
    typedef void(*DropPlugin)(Plugin*);
    typedef const char *PluginPath;
    typedef const char *PluginStatus;
    typedef const void *PluginObject;
    typedef bool PluginFound;
    typedef bool PluginLoaded;
    typedef struct PluginArtifact_ {
        Plugin         *plugin;
        PluginName     name;
        PluginFound    so_found;
        PluginLoaded   so_loaded;
        PluginObject   so_data;
        PluginPath     so_name;
        PluginStatus   status;
        MakePlugin     make_plugin;
        DropPlugin     drop_plugin;
        friend std::ostream& operator<<(std::ostream&, const PluginArtifact_);
    } PluginArtifact_;
    typedef Unique(PluginArtifact_) PluginArtifact;
    typedef std::vector<PluginArtifact> PluginArtifacts;

    class PluginManager;

    class PluginArtifactLoader {
        public:
        PluginArtifactLoader(void);
        private:
        PluginResult(int) Load(PluginArtifact&);
        friend PluginManager;
        friend std::ostream& operator<<(std::ostream&, const PluginArtifactLoader&);
    };

    // typedef std::function<PluginResult(State::state)> PluginLoader;
    // typedef std::function<PluginResult(State::state)> PluginDestroyer;

    class PluginManager {
        public:
        PluginManager(void);
        PluginResult(int) RegisterArtifact(PluginName, PluginPath);
        const PluginArtifacts& GetArtifacts(void) const;
        PluginResult(int) LoadArtifacts(void);
        void ReleaseArtifacts(void);
        private:
        std::vector<PluginArtifact>  artifacts;
        Unique(PluginArtifactLoader) artifact_loader;
        void ReleaseArtifact(std::size_t);
        void ReleaseArtifact(PluginArtifact&);
        friend std::ostream& operator<<(std::ostream&, const PluginManager&);
    };
}
