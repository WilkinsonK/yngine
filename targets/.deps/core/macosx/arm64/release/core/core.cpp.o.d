{
    files = {
        "core/core.cpp"
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
            "-Icore/include",
            "-DNDEBUG"
        }
    },
    depfiles_format = "gcc",
    depfiles = "targets/.objs/core/macosx/arm64/release/core/__cpp_core.cpp.cpp:   core/core.cpp core/include/Core.hpp\
"
}