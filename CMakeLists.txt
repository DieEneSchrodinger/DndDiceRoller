cmake_minimum_required(VERSION 3.22)

# Enter a project name
project(DndDiceRoller)
# Enter names of source files to compile
set(SOURCE main.cpp)

# Do not change anything past this point.==================================================================================

# Set the C++ standard to C++23
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find all .cpp files
file(GLOB CPP_FILES "src/*.cpp")
# Create a static library from the extra sources
set(EXTRA_SOURCES ${CPP_FILES})
if (EXTRA_SOURCES)
    add_library(helper_lib STATIC ${EXTRA_SOURCES})
    set_target_properties(helper_lib PROPERTIES CXX_EXTENSIONS OFF)
    target_compile_options(helper_lib PRIVATE -Wall -Wextra -pedantic -Wconversion)
endif ()

# Create executable
set(EXEC_NAME DndDiceRoller)
add_executable(${EXEC_NAME} ${SOURCE})
# Link the helper library (if there are extra sources)
if (EXTRA_SOURCES)
    target_link_libraries(${EXEC_NAME} PRIVATE helper_lib)
endif ()
# Link files from the inc directory
target_include_directories(${EXEC_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc)
# Set compile options
set_target_properties(${EXEC_NAME} PROPERTIES CXX_EXTENSIONS OFF)
target_compile_options(${EXEC_NAME} PRIVATE -Wall -Wextra -pedantic -Wconversion)

# Copy all txt files except CMakeLists
file(GLOB MY_FILES "*.txt")
list(FILTER MY_FILES EXCLUDE REGEX ".*CMakeLists\\.txt$")
file(COPY ${MY_FILES} DESTINATION ".")