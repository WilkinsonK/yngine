# Find ImGui from its releases and configure it accordingly.
FUNCTION(FIND_IMGUI NAME VERSION DEPS BACKENDS)
CMAKE_PARSE_ARGUMENTS(APARSED "" "NAME;VERSION" "DEPS;BACKENDS" ${ARGV})

MESSAGE("-- ImGui_Loader: Loading ImGui")
IF("${APARSED_VERSION}" STREQUAL "")
    MESSAGE("-- ImGui_Loader: No version specified; using default")
    SET(APARSED_VERSION "1.92.5")
ENDIF()
MESSAGE("-- ImGui_Loader: Using ImGui/v${APARSED_VERSION}")

IF(NOT "${APARSED_DEPS}" STREQUAL "")
    MESSAGE("-- ImGui_Loader: Linking with dependencies:")
    FOREACH(DEP ${APARSED_DEPS})
    MESSAGE("--   - ${DEP}")
    ENDFOREACH()
ELSE()
    MESSAGE("-- ImGui_Loader: No dependencies declared")
ENDIF()

LIST(FIND APARSED_DEPS "PkgConfig::SDL3" FOUND0)
LIST(FIND APARSED_DEPS "SDL3::SDL3" FOUND1)
IF((FOUND0 GREATER -1) OR (FOUND1 GREATER -1))
    MESSAGE("-- ImGui_Loader: SDL3 provided; adding to backends")
    LIST(APPEND APARSED_BACKENDS "imgui_impl_sdl3.cpp" "imgui_impl_sdlrenderer3.cpp")
ENDIF()

SET(SOURCE_NAMES "imgui.cpp" "imgui_draw.cpp" "imgui_tables.cpp" "imgui_widgets.cpp")
FOREACH(BACKEND ${APARSED_BACKENDS})
LIST(APPEND SOURCE_NAMES "backends/${BACKEND}")
ENDFOREACH()

INCLUDE(FetchContent)
FetchContent_Declare(imgui_external
	URL "https://github.com/ocornut/imgui/archive/refs/tags/v${APARSED_VERSION}.tar.gz"
	EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(imgui_external)

SET(SOURCES "")
FOREACH(SOURCE_NAME ${SOURCE_NAMES})
LIST(APPEND SOURCES "${imgui_external_SOURCE_DIR}/${SOURCE_NAME}")
ENDFOREACH()
ADD_LIBRARY(imgui ${SOURCES})

TARGET_LINK_LIBRARIES(imgui PRIVATE ${APARSED_DEPS})
TARGET_INCLUDE_DIRECTORIES(imgui PUBLIC "${imgui_external_SOURCE_DIR}" "${imgui_external_SOURCE_DIR}/backends")
SET(${ARGS_NAME} imgui PARENT_SCOPE)
RETURN()
ENDFUNCTION()
