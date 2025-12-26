set_xmakever("3.0.5")
set_version("1.0.0")
set_languages("c++23", "c23")
set_config("builddir", "targets")
add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "targets"})

-- After a plugin target has been compiled.
function plugin_post_build(target)
    os.cp(target:targetfile(), path.join(os.projectdir(), "plugins", target:name()))
end

-- Declares a plugin target.
function define_plugin(name, opts)
    opts = opts or {}
    local lang  = opts.lang or "cpp"
    local dir   = opts.dir or "plugins"
    local deps  = opts.deps or {}

    local files = {}
    if not opts.files and lang == "c" then
        files = {path.join(dir, name, "plugin.c")}
    elseif not opts.files then
        files = {path.join(dir, name, "plugin.cpp")}
    else
        files = opts.files
    end

    target(name)
        add_files(files)
        set_kind("shared")
        add_deps(deps)
        add_deps("plugin")
        after_build(plugin_post_build)
end

-- Declares a module of this project
function define_module(name, opts)
    opts = opts or {}
    local deps = opts.deps or {}
    local pkgs = opts.pkgs or {}
    local includes = path.join(name, "include")
    local internal = path.join(name, "internal")
    local kind = opts.kind or "shared"

    target(name)
        add_files(path.join(name, "src/*.cpp"))
        add_includedirs(includes, {public = true})
        add_includedirs(internal)
        add_deps(deps)
        add_packages(pkgs)
        set_kind(kind)

end

add_requires("libarchive", "log4cplus", "toml++")

define_module("core")
define_module("plugin", { pkgs = { "libarchive", "log4cplus", "toml++" }})
define_plugin("hello_world", { lang = "c", dir = "plugins" })

target("main")
    set_basename("engine")
    set_kind("binary")
    add_files("main.cpp")
    add_deps("core", "plugin")
