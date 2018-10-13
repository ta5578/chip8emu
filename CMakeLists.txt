cmake_minimum_required(VERSION 3.2)
project(chip8emu)

set(CMAKE_CXX_STANDARD 14)

# Set additional compiler flags and link directories
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    #set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX")
    set(SDL2_LIBRARY "${CMAKE_SOURCE_DIR}/lib/win/SDL2.lib" "${CMAKE_SOURCE_DIR}/lib/win/SDL2main.lib")
    link_directories(${CMAKE_SOURCE_DIR}/lib/win)
    set(SDL2_RUNTIME_LIB "${CMAKE_SOURCE_DIR}/lib/win/SDL2.dll")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror")
    link_directories(${CMAKE_SOURCE_DIR}/lib/linux)
    set(SDL2_RUNTIME_LIB "${CMAKE_SOURCE_DIR}/lib/linux/TODO")
endif()

# Set the build type (this only affects single output generators)
if(CMAKE_BUILD_TYPE MATCHES "")
    message(WARNING "CMAKE_BUILD_TYPE was not specified. The build will default to a 'release' build type. Note that this only affects single configuration generators like Make and Ninja.")
    set(CMAKE_BUILD_TYPE "release")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
else()
    message(STATUS "CMAKE_BUILD_TYPE set to ${CMAKE_BUILD_TYPE}")
    if(CMAKE_BUILD_TYPE MATCHES "debug")
        set(CMAKE_BUILD_TYPE "debug")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
    elseif(CMAKE_BUILD_TYPE MATCHES "release")
        set(CMAKE_BUILD_TYPE "release")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
    else()
        message(FATAL_ERROR "${CMAKE_BUILD_TYPE} is not supported for this generator! The only accepted values are 'debug' or 'release'!")
    endif()
endif()

message("Compiling with: ${CMAKE_CXX_COMPILER} ${CMAKE_CXX_FLAGS}")

# Set up the output directory for builds
set(CHIP8EMU_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/build")

# Set the generic runtime output directory for single generators like "make"
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CHIP8EMU_OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CHIP8EMU_OUTPUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CHIP8EMU_OUTPUT_DIR})

# Set the generic runtime output directory for multi generators like "msvc"
foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CHIP8EMU_OUTPUT_DIR})
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CHIP8EMU_OUTPUT_DIR})
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CHIP8EMU_OUTPUT_DIR})
endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)

message("Current output directory ${CHIP8EMU_OUTPUT_DIR}")

# Gather source files
include_directories(include)
file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")
file(GLOB HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "include/*.h")

add_executable(chip8emu ${HEADERS} ${SOURCES})
target_link_libraries(chip8emu ${SDL2_LIBRARY})

# Make sure we copy the required DLL to the output make_directory
add_custom_command(
        TARGET chip8emu PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${SDL2_RUNTIME_LIB}
                ${CHIP8EMU_OUTPUT_DIR})
