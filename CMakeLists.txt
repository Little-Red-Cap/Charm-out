cmake_minimum_required(VERSION 4.0)
project(out_lib)
set(target_name ${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 26)

file(GLOB_RECURSE MODULE_INTERFACE_UNITS "modules/*.cppm")

add_library(${target_name})

target_compile_definitions(${target_name}
        PRIVATE
        LOG_LEVEL_DEBUG
        OUT_ENABLE_BINARY
        OUT_ENABLE_FLOAT
)

target_sources(${target_name}
        PUBLIC
        FILE_SET cxx_modules TYPE CXX_MODULES
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        FILES ${MODULE_INTERFACE_UNITS}
)
target_include_directories(${target_name}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)
target_compile_features(${target_name} PUBLIC cxx_std_26)



add_subdirectory(examples/windows)
message("stm32 demo need go to path ‘examples/stm32f103c8’ load CMakeLists.txt")
#add_subdirectory(examples/stm32f103c8)
