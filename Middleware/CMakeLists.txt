cmake_minimum_required(VERSION 3.17)
project(Cpp_Middleware)

set(CMAKE_CXX_STANDARD 17)

#[[Set up Conan package manager]]
if (EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
    conan_basic_setup()
else ()
    message(WARNING "The file conanbuildinfo.cmake doesn't exist, you have to run conan install first - see README")
endif ()

enable_testing()
add_subdirectory(Alarming)
add_subdirectory(RESTInterface)
add_subdirectory(Database)
add_subdirectory(Utils)

add_compile_definitions(NOMINMAX)
add_definitions(-D_WIN32_WINNT=0x0A00)
add_executable(Middleware main.cpp)

target_link_libraries(Middleware ${CONAN_LIBS} Alarming Database RESTInterface Utils)

target_include_directories(Middleware PRIVATE
        ${CMAKE_SOURCE_DIR}/Alarming
        ${CMAKE_SOURCE_DIR}/Database
        ${CMAKE_SOURCE_DIR}/RESTInterface
        ${CMAKE_SOURCE_DIR}/Utils
        ${CMAKE_SOURCE_DIR}/Types)


