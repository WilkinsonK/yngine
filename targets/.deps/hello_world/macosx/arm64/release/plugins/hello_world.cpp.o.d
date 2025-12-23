{
    depfiles = "targets/.objs/hello_world/macosx/arm64/release/plugins/__cpp_hello_world.cpp.cpp:   plugins/hello_world.cpp plugin/include/Plugin.hpp\
",
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
            "-DNDEBUG"
        }
    },
    depfiles_format = "gcc",
    files = {
        "plugins/hello_world.cpp"
    }
}