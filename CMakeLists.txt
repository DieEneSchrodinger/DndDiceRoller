cmake_minimum_required(VERSION 3.22)

# Takes the source and comiles them with options based on current build type and compiler
function(compile_options SOURCE_NAME)
    if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${SOURCE_NAME} PRIVATE /W4 /permissive- /WX)
        target_compile_options(${SOURCE_NAME} PRIVATE
            $<$<CONFIG:Debug>:/Od /Zi /Ob0 /RTC1>
            $<$<CONFIG:Release>:/O2 /DNDEBUG>)
    elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR 
            CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR 
            CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
        target_compile_options(${SOURCE_NAME} PRIVATE -Wall -Wextra -Werror -Wconversion -pedantic)
        target_compile_options(${SOURCE_NAME} PRIVATE
            $<$<CONFIG:Debug>:-O0 -g -ggdb3 -fno-omit-frame-pointer>
            $<$<CONFIG:Release>:-O2 -DNDEBUG>)
    else()
        message (FATAL_ERROR "This project cannot be compiled with the compiler ${CMAKE_CXX_COMPILER_ID}.")
    endif()
endfunction()

# Set the project name and main source file(s)
project(DndDiceRoller)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find all src/.cpp files and create a static library from them
file(GLOB CPP_FILES "src/*.cpp")
set(EXTRA_SOURCES ${CPP_FILES})
if (EXTRA_SOURCES)
    add_library(srcs STATIC ${EXTRA_SOURCES})
    target_include_directories(srcs PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/inc)
    compile_options(srcs)
endif ()

# Create executable
set(EXEC_NAME DndDiceRoller)
add_executable(${EXEC_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
#Add additional sources and link libraries
if (EXTRA_SOURCES)
    target_link_libraries(${EXEC_NAME} PRIVATE srcs)
endif ()
target_include_directories(${EXEC_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc)
compile_options(${EXEC_NAME})

if (WIN32 OR CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(EXE_PATH "${CMAKE_CURRENT_BINARY_DIR}/${EXEC_NAME}.exe")
    set(BUNDLE_DIR "${CMAKE_CURRENT_BINARY_DIR}/bundle")
    file(MAKE_DIRECTORY ${BUNDLE_DIR})

    find_program(MINGW_STRIP x86_64-w64-mingw32-strip)

    # Copy the .exe to the bundle directory
    add_custom_command(
        TARGET ${EXEC_NAME}  POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${EXE_PATH}
                "${BUNDLE_DIR}/${EXEC_NAME}.exe")

    # Strip the executable
    execute_process(COMMAND ${MINGW_STRIP} -s ${EXE_PATH}
                    RESULT_VARIABLE STRIP_RESULT
                    OUTPUT_QUIET)

    # Copy the dlls to the bundle directory
    file(GLOB DLL_FILES "${CMAKE_CURRENT_SOURCE_DIR}/dlls/*.dll")

    file(COPY ${DLL_FILES} DESTINATION ${BUNDLE_DIR})
endif()

# Copy all txt files except CMakeLists
file(GLOB MY_FILES "*.txt")
list(FILTER MY_FILES EXCLUDE REGEX ".*CMakeLists\\.txt$")
file(COPY ${MY_FILES} DESTINATION ".")