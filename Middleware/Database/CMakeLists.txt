file(GLOB Database_SRC CONFIGURE_DEPENDS "*.cpp")

add_library(Database ${Database_SRC} IDatabaseFactory.h DatabaseFactory.cpp DatabaseFactory.h)

if (WIN32)
    add_compile_definitions(NOMINMAX)
    add_definitions(-D_WIN32_WINNT=0x0A00)
endif ()
