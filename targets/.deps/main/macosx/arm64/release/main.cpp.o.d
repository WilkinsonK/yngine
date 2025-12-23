{
    depfiles_format = "gcc",
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
            "-fvisibility=hidden",
            "-fvisibility-inlines-hidden",
            "-O3",
            "-std=c++23",
            "-Icore/include",
            "-Iplugin/include",
            "-DNDEBUG"
        }
    },
    depfiles = "targets/.objs/main/macosx/arm64/release/__cpp_main.cpp.cpp: main.cpp   plugin/include/Plugin.hpp plugin/include/State.hpp   plugin/include/Traits.hpp\
",
    files = {
        "main.cpp"
    }
}