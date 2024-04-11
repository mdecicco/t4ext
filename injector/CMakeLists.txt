cmake_minimum_required(VERSION 3.15)
project(injector)

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

add_sources("src"           ".")
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

add_executable(injector ${all_sources})

target_include_directories(injector PUBLIC src)
target_include_directories(injector PUBLIC "../deps/utils/include")

target_link_directories(injector PUBLIC lib)

SET(debug_libs 
    utils
)

SET(release_libs
    utils
)

# target_link_libraries(injector ${debug_libs})
target_link_libraries(injector ${release_libs})
set_target_properties(injector PROPERTIES LINK_FLAGS "/ignore:4099")

set_property(TARGET injector PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG}")

add_definitions(-D_CRT_NO_VA_START_VALIDATION)