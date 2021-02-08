include(GoogleTest)
link_directories(${CONAN_LIB_DIRS})

file(GLOB MachineLearningTests_SRC CONFIGURE_DEPENDS "*.cpp")

add_executable(MachineLearningTests ${MachineLearningTests_SRC})

target_link_libraries(MachineLearningTests ${CONAN_LIBS} Alarming Database)

target_include_directories(MachineLearningTests PRIVATE
        ${CMAKE_SOURCE_DIR}/Alarming
        ${CMAKE_SOURCE_DIR}/Database
        ${CMAKE_SOURCE_DIR}/Hypodermic
        ${CMAKE_SOURCE_DIR}/Utils
        ${CMAKE_SOURCE_DIR}/Types)

gtest_add_tests(TARGET MachineLearningTests SOURCES MachineLearningTests.cpp)

if (WIN32)
    add_compile_definitions(NOMINMAX)
    add_definitions(-D_WIN32_WINNT=0x0A00)
endif ()
