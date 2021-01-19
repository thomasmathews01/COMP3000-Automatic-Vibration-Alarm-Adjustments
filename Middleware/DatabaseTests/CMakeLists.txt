include(GoogleTest)
link_directories(${CONAN_LIB_DIRS})

file(GLOB DatabaseTests_SRC CONFIGURE_DEPENDS "*.cpp")

add_executable(DatabaseTests ${DatabaseTests_SRC} MockDatabaseFactory.h)

target_link_libraries(DatabaseTests ${CONAN_LIBS} Database)

target_include_directories(DatabaseTests PRIVATE
        ${CMAKE_SOURCE_DIR}/Hypodermic
        ${CMAKE_SOURCE_DIR}/Database
        ${CMAKE_SOURCE_DIR}/Types)

gtest_add_tests(TARGET DatabaseTests SOURCES DatabaseTests.cpp DatabaseInitialiserTests.cpp)