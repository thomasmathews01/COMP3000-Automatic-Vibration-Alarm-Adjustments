file(GLOB RESTInterface_SRC CONFIGURE_DEPENDS "Utils/*.cpp" "NetworkRequestHandlers/*.cpp" "*.cpp")

add_library(RESTInterface ${RESTInterface_SRC})

add_compile_definitions(NOMINMAX)
add_definitions(-D_WIN32_WINNT=0x0A00)