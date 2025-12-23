-- set_xmakever("3.0.5")
set_xmakever("2.9.6")
set_version("1.0.0")
set_languages("c++23")
set_config("builddir", "targets")
add_rules("mode.debug", "mode.release")
add_rules("plugin.compile_commands.autoupdate", {outputdir = "."})

-- add_requires("log4cplus")
target("core")
    add_files("core/*.cpp")
    add_includedirs("core/include", {public = true})
    -- add_packages("log4cplus")
    set_kind("shared")

target("plugin")
    add_files("plugin/*.cpp")
    add_includedirs("plugin/include", {public = true})
    add_includedirs("plugin/internal")
    -- add_packages("log4cplus")
    set_kind("shared")

-- Sample plugin. Does nothing except print, "Hello, World!"
-- when installed.
target("hello_world")
    add_files("plugins/hello_world.cpp")
    set_kind("shared")
    add_deps("plugin")

target("main")
    set_basename("engine")
    set_kind("binary")
    add_files("main.cpp")
    add_deps("core", "plugin")
