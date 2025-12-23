{
    values = {
        "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++",
        {
            "-arch",
            "arm64",
            "-target",
            "arm64-apple-macos15.6",
            "-isysroot",
            "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX26.0.sdk",
            "-lz",
            "-Ltargets/macosx/arm64/release",
            "-Xlinker",
            "-rpath",
            "-Xlinker",
            "@loader_path",
            "-Wl,-x",
            "-Wl,-dead_strip",
            "-lcore",
            "-lplugin"
        }
    },
    files = {
        "targets/.objs/main/macosx/arm64/release/main.cpp.o"
    }
}