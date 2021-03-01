cmake_minimum_required(VERSION 3.17)
project(Cpp_CUDAModel)

set(CMAKE_CXX_STANDARD 17)

#[[Set up Conan package manager]]
if (EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
else ()
    message(WARNING "The file conanbuildinfo.cmake doesn't exist, you have to run conan install first - see README")
endif ()

enable_testing()
add_subdirectory(Model)
add_subdirectory(Tests)

if (WIN32)
    add_compile_definitions(NOMINMAX)
    add_definitions(-D_WIN32_WINNT=0x0A00)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif ()

add_executable(CUDAModel main.cpp)

target_link_libraries(CUDAModel PRIVATE ${CONAN_LIBS} Model)

target_include_directories(CUDAModel PRIVATE
        ${CMAKE_SOURCE_DIR}/Model
        ${CMAKE_SOURCE_DIR}/Test
        )