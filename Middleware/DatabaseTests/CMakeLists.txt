include(GoogleTest)

file(GLOB DatabaseTests_SRC CONFIGURE_DEPENDS "*.cpp")

add_executable(DatabaseTests ${DatabaseTests_SRC} DataStorageTests.cpp)

target_link_libraries(DatabaseTests ${CONAN_LIBS} Database)

target_include_directories(DatabaseTests PRIVATE
        ${CMAKE_SOURCE_DIR}/Hypodermic
        ${CMAKE_SOURCE_DIR}/Database
        ${CMAKE_SOURCE_DIR}/Types
        )

gtest_add_tests(TARGET DatabaseTests SOURCES DatabaseTests.cpp DatabaseInitialiserTests.cpp ConfigStorageTests.cpp DataStorageTests.cpp)

if (WIN32)
    add_compile_definitions(NOMINMAX)
    add_definitions(-D_WIN32_WINNT=0x0A00)
endif ()
