{
    files = {
        "targets/.objs/plugin/macosx/arm64/release/plugin/plugin_display.cpp.o",
        "targets/.objs/plugin/macosx/arm64/release/plugin/plugin_manager.cpp.o",
        "targets/.objs/plugin/macosx/arm64/release/plugin/plugin_base.cpp.o",
        "targets/.objs/plugin/macosx/arm64/release/plugin/plugin_loader.cpp.o"
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