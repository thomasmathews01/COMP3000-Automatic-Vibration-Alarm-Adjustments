include(GoogleTest)
link_directories(${CONAN_LIB_DIRS})

file(GLOB RestInterfaceTests_SRC CONFIGURE_DEPENDS "*.cpp")

add_executable(RestInterfaceTests ${RestInterfaceTests_SRC})

target_link_libraries(RestInterfaceTests ${CONAN_LIBS} RESTInterface)

target_include_directories(RestInterfaceTests PRIVATE
        ${CMAKE_SOURCE_DIR}/RestInterface
        ${CMAKE_SOURCE_DIR}/Types)

gtest_add_tests(TARGET RestInterfaceTests SOURCES RestInterfaceTests.cpp)