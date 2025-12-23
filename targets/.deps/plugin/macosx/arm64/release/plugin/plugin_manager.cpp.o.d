{
    depfiles = "targets/.objs/plugin/macosx/arm64/release/plugin/__cpp_plugin_manager.cpp.cpp:   plugin/plugin_manager.cpp plugin/include/Plugin.hpp\
",
    depfiles_format = "gcc",
    files = {
        "plugin/plugin_manager.cpp"
    },
    values = {
        "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++",
        {
            "-Qunused-arguments",
            "-arch",
            "arm64",
            "-target",
            "arm64-apple-macos15.6",
            "-isysroot",
            "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX26.0.sdk",
            "-fPIC",
            "-O3",
            "-std=c++23",
            "-Iplugin/include",
            "-Iplugin/internal",
            "-DNDEBUG"
        }
    }
}