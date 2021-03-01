include(GoogleTest)
link_directories(${CONAN_LIB_DIRS})

add_executable(Tests tests.cpp)

target_link_libraries(Tests ${CONAN_LIBS} Model)

target_include_directories(Tests PRIVATE
        ${CMAKE_SOURCE_DIR}/Model)

gtest_add_tests(TARGET Tests SOURCES Tests.cpp )

if (WIN32)
    add_compile_definitions(NOMINMAX)
    add_definitions(-D_WIN32_WINNT=0x0A00)
endif ()
