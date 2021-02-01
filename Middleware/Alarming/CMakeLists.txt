file(GLOB Alarming_SRC CONFIGURE_DEPENDS "Calculators/*.cpp" "Statistics/*.cpp" "*.cpp")

add_library(Alarming ${Alarming_SRC} Statistics/IStatsCalcFactory.h Statistics/StatsCalcFactory.cpp Statistics/StatsCalcFactory.h)

if (WIN32)
    add_compile_definitions(NOMINMAX)
    add_definitions(-D_WIN32_WINNT=0x0A00)
endif ()

target_include_directories(Alarming PRIVATE
        ${CMAKE_SOURCE_DIR}/Hypodermic
        ${CMAKE_SOURCE_DIR}/Utils
        ${CMAKE_SOURCE_DIR}/Database
        ${CMAKE_SOURCE_DIR}/taskflow/taskflow
        ${CMAKE_SOURCE_DIR}/Types)