include(GoogleTest)
link_directories(${CONAN_LIB_DIRS})

file(GLOB AlarmingTests_SRC CONFIGURE_DEPENDS "*.cpp")

add_executable(AlarmingTests ${AlarmingTests_SRC} Stubs/StubClock.h Stubs/StubDataAccess.cpp)

target_link_libraries(AlarmingTests ${CONAN_LIBS} Alarming)

target_include_directories(AlarmingTests PRIVATE
        ${CMAKE_SOURCE_DIR}/Alarming
        ${CMAKE_SOURCE_DIR}/Database
        ${CMAKE_SOURCE_DIR}/Utils
        ${CMAKE_SOURCE_DIR}/Hypodermic
        ${CMAKE_SOURCE_DIR}/Types
        )


if (WIN32)
    add_compile_definitions(NOMINMAX)
    add_definitions(-D_WIN32_WINNT=0x0A00)
endif ()


gtest_add_tests(TARGET AlarmingTests SOURCES alarmTests.cpp StatisticsTests.cpp)