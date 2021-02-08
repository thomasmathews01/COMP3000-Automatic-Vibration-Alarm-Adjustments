file(GLOB Utils_SRC CONFIGURE_DEPENDS "*.cpp")

add_library(Utils ${Utils_SRC} spdloghelper.h ../RESTInterface/Utils/CrowUtils.cpp ../RESTInterface/Utils/CrowUtils.h)

#[[
add_compile_definitions(NOMINMAX)
add_definitions(-D_WIN32_WINNT=0x0A00)]]

target_include_directories(Utils PRIVATE
        ${CMAKE_SOURCE_DIR}/Types)
