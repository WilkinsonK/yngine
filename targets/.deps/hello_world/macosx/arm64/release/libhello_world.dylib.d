{
    files = {
        "targets/.objs/hello_world/macosx/arm64/release/plugins/hello_world.cpp.o"
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
            "-Ltargets/macosx/arm64/release",
            "-Wl,-x",
            "-Wl,-dead_strip",
            "-lplugin"
        }
    }
}