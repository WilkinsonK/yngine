{
    files = {
        "targets/.objs/core/macosx/arm64/release/core/core.cpp.o"
    },
    values = {
        "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++",
        {
            "-shared",
            "-arch",
            "arm64",
            "-target",
            "arm64-apple-macos15.6",
            "-isysroot",
            "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX26.0.sdk",
            "-lz",
            "-fPIC",
            "-Wl,-x",
            "-Wl,-dead_strip"
        }
    }
}