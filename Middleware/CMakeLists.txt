cmake_minimum_required(VERSION 3.17)
project(Cpp_Middleware)

set(CMAKE_CXX_STANDARD 17)
find_package(cpprestsdk REQUIRED)
find_package(spdlog REQUIRED)

add_executable(Cpp_Middleware main.cpp "REST Interface/AlternativeServer.cpp" "REST Interface/AlternativeServer.h" "REST Interface/RequestHandler.cpp" "REST Interface/RequestHandler.h" "Database/IDatabase.cpp" "Database/IDatabase.h" "REST Interface/Model/types.h" "Database/Database.cpp" "Database/Database.h" Database/Helpers/sqlite3pp.h Database/Helpers/sqlite3pp.ipp "REST Interface/crow.h" "REST Interface/AnotherAlternativeServer.cpp" "REST Interface/AnotherAlternativeServer.h")
target_link_libraries(Cpp_Middleware PRIVATE cpprestsdk::cpprest libsqlite3.tbd spdlog::spdlog)
include_directories(/usr/local/Cellar/doctest/2.4.0/include /usr/local/Cellar/range-v3/0.11.0/include /usr/local/Cellar/boost/1.74.0 /usr/local/opt/sqlite/include)
