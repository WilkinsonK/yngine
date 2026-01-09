#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>

#include "archive.h"
#include "archive_entry.h"

#include "plugin.hpp"
#include "plugin_internal.hpp"

namespace __plugin_root::package {
    using Manifest = manifest::Manifest;
    typedef std::function<StateInfo(const artifact::Artifact&)> BuildModeHandler;

    Package Body::New(void) {
        return std::make_unique<Body>();
    }

    Package Body::FromManifest(std::filesystem::path path) {
        return Body::Builder(path).Build();
    }

    std::filesystem::path Body::GetManifest(void) {
        return GetSourceDir() / (manifest != "" ? manifest : "plugin.toml");
    }

    std::filesystem::path Body::GetOutputDir(void) {
        return output_dir.value_or(GetSourceDir());
    }

    std::filesystem::path Body::GetSourceDir(void) {
        return source_dir
            .or_else([](){ return std::optional(std::filesystem::current_path()); })
            .transform([](std::string sd){
                return sd != "" ?
                    std::filesystem::path(sd) :
                    std::filesystem::current_path();
            })
            .value();
    }

    std::filesystem::path Body::GetOutputName(void) {
        std::stringstream ss;
        ss << (name != "" ? name : "yplugin_package")
           << "-"
           << (version != "" ? version : "0.0.0")
           << ".yplugin";
        return GetOutputDir() / ss.str();
    }

    Body::Builder::Builder(void) : inner(std::make_unique<Body>()) {}

    Body::Builder::Builder(std::filesystem::path manifest) : Builder() {
        Manifest m;
        
        WithManifestName(manifest);
        if (!std::filesystem::exists(manifest)) return;

        try {
            m = Manifest::Load(manifest);
        } catch (const toml::parse_error& err) {
            return;
        }

        WithName(m.plugin.name);
        WithVersion(m.plugin.version);
        auto root = manifest.parent_path();
        for (auto name : m.module.include ) AddInclude({ name, root / name });
        // These files should always be a part of the
        // package.
        AddInclude({ manifest.filename(), manifest });
        AddInclude({ m.module.entry.filename(), m.module.entry });
    }

    Package Body::Builder::Build(void) {
        return std::move(inner);
    }

    Body::Builder& Body::Builder::AddInclude(Entry entry) {
        inner->included_files.push_back(entry);
        return *this;
    }

    Body::Builder& Body::Builder::WithManifestName(std::string manifest) {
        inner->manifest = manifest;
        return *this;
    }

    Body::Builder& Body::Builder::WithName(std::string name) {
        inner->name = name;
        return *this;
    }

    Body::Builder& Body::Builder::WithOutputDir(std::filesystem::path out) {
        inner->output_dir = out;
        return *this;
    }

    Body::Builder& Body::Builder::WithSourceDir(std::filesystem::path src) {
        inner->source_dir= src;
        return *this;
    }

    Body::Builder& Body::Builder::WithVersion(std::string vers) {
        inner->version = vers;
        return *this;
    }

    StateInfo BuildWithMake(const artifact::Artifact& source) {
        if (!std::filesystem::exists(source->manifest))
            return MAN_NOMAN & source->manifest;

        std::filesystem::path p = source->manifest;
        std::stringstream ss;
        ss << "make -C " << p.parent_path();
        std::system(ss.str().c_str());

        return NONE;
    }

    StateInfo Build(const artifact::Artifact& source, BuildModeHandler handler) {
        return handler(source);
    }

    StateInfo Build(const Context& ctx) {
        // Build the module according to build mode.
        // Check that the build was sucessful.
        // Check that the resulting entry point matches manifest.
        auto mode = ctx.artifact->GetMode();
        if (mode == BM_MAKE) return Build(ctx.artifact, BuildWithMake);
        return BLD_MODEUNK;
    }

    StateInfo CreateEntry(struct archive *a, Entry& e) {
        StateInfo state;
        struct stat st;
        struct archive_entry *entry;
        std::stringstream ss;
        std::string dat;

        stat(e.source.c_str(), &st);
        entry = archive_entry_new();
        if (entry == nullptr) goto handle_error;
        archive_entry_set_pathname(entry, e.alias.c_str());
        archive_entry_set_size(entry, st.st_size);
        archive_entry_set_filetype(entry, AE_IFREG);
        archive_entry_set_perm(entry, 0644);
        archive_write_header(a, entry);
        if (archive_errno(a)) goto handle_error;

        ss << std::ifstream(e.source).rdbuf();
        dat = ss.str();
        ss.clear();
        archive_write_data(a, dat.c_str(), dat.size());
        goto close_entry;

        return_state:
        return state;
        handle_error:
        state = BLD_MALPKG & archive_error_string(a);
        close_entry:
        if (entry != nullptr) archive_entry_free(entry);
        goto return_state;
    }

    StateInfo Pack(const Context& ctx) {
        if (!std::filesystem::exists(ctx.package->GetManifest()))
            return MAN_NOMAN & ctx.package->manifest;

        StateInfo state;
        struct archive *a;

        // Initialize archive.
        // Set format and filter, then open the archive file
        // for writing.
        a = archive_write_new();
        if (a == nullptr) goto handle_error;
        archive_write_add_filter_gzip(a);
        if (archive_errno(a)) goto handle_error;
        archive_write_set_format_gnutar(a);
        if (archive_errno(a)) goto handle_error;
        archive_write_open_filename(a, ctx.package->GetOutputName().c_str());

        // Write entries from package.
        for (auto e : ctx.package->included_files) {
            state = CreateEntry(a, e);
            if (state != NONE) break;
        }
        goto close_archive;

        // Handle cleanups and errors before return.
        return_state:
        return state;
        handle_error:
        state = BLD_MALPKG & archive_error_string(a);
        close_archive:
        if (a != nullptr) {
            archive_write_close(a);
            archive_write_free(a);
        }
        goto return_state;
    }

    StateInfo Unpack(const Context&) {
        return NONE;
    }

    Package New(void) {
        return Body::New();
    }

    Package FromManifest(std::filesystem::path path) {
        return Body::FromManifest(path);
    }
}
