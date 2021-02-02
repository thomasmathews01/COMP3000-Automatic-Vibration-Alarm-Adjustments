include(GoogleTest)
link_directories(${CONAN_LIB_DIRS})

file(GLOB AlarmingTests_SRC CONFIGURE_DEPENDS "*.cpp" "Stubs/*.cpp" "Mocks/*.cpp")

add_executable(AlarmingTests ${AlarmingTests_SRC} MachineLearningTests.cpp)

target_link_libraries(AlarmingTests ${CONAN_LIBS} Alarming)

target_include_directories(AlarmingTests PRIVATE
        ${CMAKE_SOURCE_DIR}/Alarming
        ${CMAKE_SOURCE_DIR}/Database
        ${CMAKE_SOURCE_DIR}/Utils
        ${CMAKE_SOURCE_DIR}/Hypodermic
        ${CMAKE_SOURCE_DIR}/Types
        ${CMAKE_SOURCE_DIR}/taskflow/taskflow
        )


if (WIN32)
    add_compile_definitions(NOMINMAX)
    add_definitions(-D_WIN32_WINNT=0x0A00)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif ()


gtest_add_tests(TARGET AlarmingTests SOURCES alarmTests.cpp StatisticsTests.cpp AlarmHandlerTests.cpp MachineLearningTests.cpp)