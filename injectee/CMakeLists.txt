cmake_minimum_required(VERSION 3.15)
project(t4ext)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set (all_sources "")
macro(add_sources group dir)
    set(_all "")

    file(GLOB _inc "src/${dir}/*.h" "src/${dir}/*.hpp" "src/${dir}/*.inl")
    list(APPEND _all ${_inc})

    file(GLOB _src "src/${dir}/*.cpp")
    list(APPEND _all ${_src})

    list(APPEND all_sources ${_all})
    string(REPLACE "/" "\\" group_path "${group}")
    source_group(${group_path} FILES ${_all})

    unset(_inc)
    unset(_src)
    unset(_all)
endmacro(add_sources)

file(GLOB imgui_src "../deps/imgui/*.cpp")
list(APPEND all_sources ${imgui_src})
list(APPEND all_sources "../deps/imgui/backends/imgui_impl_dx9.cpp")
list(APPEND all_sources "../deps/imgui/backends/imgui_impl_win32.cpp")

add_sources("src"           ".")
add_sources("core"          "core")
add_sources("hooks"         "hooks")
add_sources("script"        "script")
add_sources("events"        "events")
add_sources("builtin"       "builtin")
add_sources("bindings"      "bindings")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_library(t4ext SHARED ${all_sources})

target_include_directories(t4ext PUBLIC src)
target_include_directories(t4ext PUBLIC "../deps/utils/include")
target_include_directories(t4ext PUBLIC "../deps/v8/include")
target_include_directories(t4ext PUBLIC "../deps/imgui")
target_include_directories(t4ext PUBLIC "../deps/minhook/include")
target_include_directories(t4ext PUBLIC "../build/deps/libffi/include")
target_link_directories(t4ext PUBLIC "../deps/v8/lib")
target_link_directories(t4ext PUBLIC "../lib")

target_link_directories(t4ext PUBLIC lib)

SET(debug_libs 
    utils
    minhook
)

SET(release_libs
    utils
    minhook
    v8
    v8_libplatform
    Psapi
    ffi_static
    d3d9
)

# target_link_libraries(t4ext ${debug_libs})
target_link_libraries(t4ext ${release_libs})
set_target_properties(t4ext PROPERTIES LINK_FLAGS "/ignore:4099")

set_property(TARGET t4ext PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}")

add_definitions(-D_CRT_NO_VA_START_VALIDATION)