cmake_minimum_required(VERSION 3.2)
project(chip8emu)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wpedantic")
message("Compiling with: ${CMAKE_CXX_COMPILER} ${CMAKE_CXX_FLAGS}")

# Gather source files
include_directories(include)
file(GLOB SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "src/*.cpp")
file(GLOB HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "include/*.h")

# Create a static library from source so tests (if available) can use it
add_library(chip8emuS STATIC ${HEADERS} ${SOURCES})

# Look for UnitTest++ and decide whether to run unit tests
find_package(UnitTest++)

# Build the test suite if we found UnitTest++
if (${UnitTest++_FOUND})
    message("UnitTest++ found: ${UnitTest++_DIR}. Building unit tests...")

    file(GLOB TEST_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} test/*.cpp test/*.h)

    include_directories(${UTPP_INCLUDE_DIRS})
    add_executable(Testchip8emu ${TEST_SRCS})
    target_link_libraries(Testchip8emu UnitTest++ chip8emuS)

    # run unit tests as a post build step
    add_custom_command(TARGET Testchip8emu POST_BUILD COMMAND Testchip8emu
        COMMENT "Running unit tests...")

    # now create a tests target so we can run unit tests as ad-hoc
    enable_testing()
    add_test(NAME Testchip8emu COMMAND Testchip8emu)
else ()
    message("UnitTest++ was not found so unit tests will not be built and run!")
endif()

# Find SDL libraries
# Silence CMake about a "trailing/leading whitespace" error
cmake_policy(SET CMP0004 OLD)
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

add_executable(chip8emu ${SOURCES})
target_link_libraries(chip8emu ${SDL2_LIBRARIES})
