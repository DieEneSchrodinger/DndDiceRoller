cmake_minimum_required(VERSION 3.22)

# Takes the source and comiles them with options based on current build type and compiler
function(compile_options SOURCE_NAME)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${SOURCE_NAME} PRIVATE /W4 /permissive- /WX)
        if (CMAKE_BUILD_TYPE STREQUAL "Debug")
            target_compile_options(${SOURCE_NAME} PRIVATE /Od /Zi /Ob0 /RTC1)
        else()
            target_compile_options(${SOURCE_NAME} PRIVATE /O2 /DNDEBUG)
        endif()
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR 
            CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR 
            CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        target_compile_options(${SOURCE_NAME} PRIVATE -Wall -Wextra -Werror -Wconversion -pedantic)
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            target_compile_options(${SOURCE_NAME} PRIVATE -O0 -g -ggdb3 -fno-omit-frame-pointer)
        else()
            target_compile_options(${SOURCE_NAME} PRIVATE -O2 -DNDEBUG)
        endif()
    else()
        message (FATAL_ERROR "This project cannot be compiled with the compiler ${CMAKE_CXX_COMPILER_ID}.")
    endif()
endfunction()

# Set the project name and main source file(s)
project(DndDiceRoller)
set(SOURCE main.cpp)

# Set the C++ standard to C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find all .cpp files
file(GLOB CPP_FILES "src/*.cpp")
# Create a static library from the extra sources
set(EXTRA_SOURCES ${CPP_FILES})
if (EXTRA_SOURCES)
    add_library(srcs STATIC ${EXTRA_SOURCES})
    compile_options(srcs)
endif ()

# Create executable
set(EXEC_NAME DndDiceRoller)
add_executable(${EXEC_NAME} ${SOURCE})
# Link the helper library if there are extra sources
if (EXTRA_SOURCES)
    target_link_libraries(${EXEC_NAME} PRIVATE srcs)
endif ()
# Link files from the inc directory
target_include_directories(${EXEC_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc)
# Set compile options
compile_options(${EXEC_NAME})

# Copy all txt files except CMakeLists
file(GLOB MY_FILES "*.txt")
list(FILTER MY_FILES EXCLUDE REGEX ".*CMakeLists\\.txt$")
file(COPY ${MY_FILES} DESTINATION ".")