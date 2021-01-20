file(GLOB Utils_SRC CONFIGURE_DEPENDS "*.cpp")

add_library(Utils ${Utils_SRC})

#[[
add_compile_definitions(NOMINMAX)
add_definitions(-D_WIN32_WINNT=0x0A00)]]
