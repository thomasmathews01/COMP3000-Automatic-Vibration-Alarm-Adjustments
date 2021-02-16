include(GoogleTest)
link_directories(${CONAN_LIB_DIRS})

file(GLOB RestInterfaceTests_SRC CONFIGURE_DEPENDS "*.cpp")

add_executable(RestInterfaceTests ${RestInterfaceTests_SRC})

target_link_libraries(RestInterfaceTests ${CONAN_LIBS} RESTInterface Utils)

target_include_directories(RestInterfaceTests PRIVATE
        ${CMAKE_SOURCE_DIR}/RestInterface
        ${CMAKE_SOURCE_DIR}/Database
        ${CMAKE_SOURCE_DIR}/Utils
        ${CMAKE_SOURCE_DIR}/Types)

gtest_add_tests(TARGET RestInterfaceTests SOURCES RestInterfaceTests.cpp AlarmActivationsTest.cpp CurrentAlarmState.cpp AlarmSettingsRequestTests.cpp)

if (WIN32)
    add_compile_definitions(NOMINMAX)
    add_definitions(-D_WIN32_WINNT=0x0A00)
endif ()
