file(GLOB RESTInterface_SRC CONFIGURE_DEPENDS "Utils/*.cpp" "NetworkRequestHandlers/*.cpp" "*.cpp")

add_library(RESTInterface ${RESTInterface_SRC} IServer.h)

add_compile_definitions(NOMINMAX)
add_definitions(-D_WIN32_WINNT=0x0A00)


target_include_directories(RESTInterface PRIVATE
        ${CMAKE_SOURCE_DIR}/Hypodermic
        ${CMAKE_SOURCE_DIR}/Database
        ${CMAKE_SOURCE_DIR}/Types)