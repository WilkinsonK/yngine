ADD_LIBRARY(EngineUI SHARED)
TARGET_SOURCES(EngineUI PRIVATE src/EngineUI.cpp)
# TARGET_SOURCES(
#     EngineUI
#     PRIVATE
#         src/EngineUI_SDL/Global.cpp
#         src/EngineUI_SDL/Globals.cpp
#         src/EngineUI_SDL/Renderer.cpp
#         src/EngineUI_SDL/State.cpp
# )
TARGET_INCLUDE_DIRECTORIES(EngineUI PUBLIC include)
TARGET_INCLUDE_DIRECTORIES(EngineUI PRIVATE internal)
TARGET_LINK_LIBRARIES(EngineUI PRIVATE imgui PkgConfig::log4cplus PkgConfig::SDL3)
