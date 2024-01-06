# CMakeList.txt : CMake project for test-cmake, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("test_cmake")

# Add source to this project's executable.
set(GLAD_INCLUDE "D:/TOOLS/GLAD/include/")
set(GLFW_INCLUDE "D:/TOOLS/GLFW/include/")
set(GLFW_LIB "D:/TOOLS/GLFW/lib/")

set(HEADER_FILES
    ${GLAD_INCLUDE}/glad/glad.h
    ${GLFW_INCLUDE}/GLFW/glfw3.h
)

set(SOURCE_FILES
    "main.cpp"
    "glad.c"
    ${HEADER_FILES}
)

add_executable(test_cmake ${SOURCE_FILES})

# import OpenGL library
find_package(OpenGL REQUIRED)

if (OPENGL_FOUND)
    message("OpenGL found")
else (OPENGL_FOUND)
    message("OpenGL not found")
endif()

# specify directories for external header files
target_include_directories(test_cmake PRIVATE ${GLAD_INCLUDE} ${GLFW_INCLUDE})

# specify directories for external libraries
target_link_directories(test_cmake PRIVATE ${GLFW_LIB})

# specify libraries to link
target_link_libraries(test_cmake PRIVATE glfw3.lib ${OPENGL_gl_LIBRARY})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET test_cmake PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
