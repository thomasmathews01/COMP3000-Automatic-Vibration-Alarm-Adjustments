cmake_minimum_required(VERSION 3.17)
project(Cpp_Middleware)

set(CMAKE_CXX_STANDARD 17)


IF (WIN32)

    #[[Fetch the windows version, which BOOST looks for]]
    macro(get_WIN32_WINNT version)
        if (WIN32 AND CMAKE_SYSTEM_VERSION)
            set(ver ${CMAKE_SYSTEM_VERSION})
            string(REPLACE "." "" ver ${ver})
            string(REGEX REPLACE "([0-9])" "0\\1" ver ${ver})

            set(${version} "0x${ver}")
        endif()
    endmacro()

    get_WIN32_WINNT(ver)
    add_definitions(-D_WIN32_WINNT=${ver})

    #[[Set up Conan package manager]]
    if(EXISTS ${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
        include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
        conan_basic_setup()
    else()
        message(WARNING "The file conanbuildinfo.cmake doesn't exist, you have to run conan install first")
    endif()

    add_executable(Cpp_Middleware main.cpp "Database/IDatabase.h" "REST Interface/Model/types.h" "Database/Database.cpp" "Database/Database.h" Database/Helpers/sqlite3pp.h Database/Helpers/sqlite3pp.ipp "REST Interface/Server.cpp" "REST Interface/Server.h" Database/TestDatabase.h "REST Interface/NetworkRequestHandlers/HelloWorldRequest.cpp" "REST Interface/NetworkRequestHandlers/HelloWorldRequest.h" "REST Interface/NetworkRequestHandlers/GetSitesRequest.cpp" "REST Interface/NetworkRequestHandlers/GetSitesRequest.h" "REST Interface/NetworkRequestHandlers/SiteInformationRequest.cpp" "REST Interface/NetworkRequestHandlers/SiteInformationRequest.h" "REST Interface/NetworkRequestHandlers/MachineInformationRequest.cpp" "REST Interface/NetworkRequestHandlers/MachineInformationRequest.h" InternalClock.cpp InternalClock.h "REST Interface/NetworkRequestHandlers/CurrentTimeRequest.cpp" "REST Interface/NetworkRequestHandlers/CurrentTimeRequest.h" "REST Interface/NetworkRequestHandlers/AvailableDataTypesRequest.cpp" "REST Interface/NetworkRequestHandlers/AvailableDataTypesRequest.h" "REST Interface/NetworkRequestHandlers/GetDataRequest.cpp" "REST Interface/NetworkRequestHandlers/GetDataRequest.h" "REST Interface/NetworkRequestHandlers/all.h" "REST Interface/NetworkRequestHandlers/GetStatePeriodsRequest.cpp" "REST Interface/NetworkRequestHandlers/GetStatePeriodsRequest.h" "REST Interface/Model/stateTypes.h" "REST Interface/NetworkRequestHandlers/AddStatePeriodRequest.cpp" "REST Interface/NetworkRequestHandlers/AddStatePeriodRequest.h" "REST Interface/Conversions.cpp" "REST Interface/Conversions.h")
    target_link_libraries(Cpp_Middleware ${CONAN_LIBS})
ELSEIF ()
    find_package(spdlog REQUIRED)
    find_package(RapidJSON REQUIRED)
    add_executable(Cpp_Middleware main.cpp "Database/IDatabase.h" "REST Interface/Model/types.h" "Database/Database.cpp" "Database/Database.h" Database/Helpers/sqlite3pp.h Database/Helpers/sqlite3pp.ipp "REST Interface/Server.cpp" "REST Interface/Server.h" Database/TestDatabase.h "REST Interface/NetworkRequestHandlers/HelloWorldRequest.cpp" "REST Interface/NetworkRequestHandlers/HelloWorldRequest.h" "REST Interface/NetworkRequestHandlers/GetSitesRequest.cpp" "REST Interface/NetworkRequestHandlers/GetSitesRequest.h" "REST Interface/NetworkRequestHandlers/SiteInformationRequest.cpp" "REST Interface/NetworkRequestHandlers/SiteInformationRequest.h" "REST Interface/NetworkRequestHandlers/MachineInformationRequest.cpp" "REST Interface/NetworkRequestHandlers/MachineInformationRequest.h" InternalClock.cpp InternalClock.h "REST Interface/NetworkRequestHandlers/CurrentTimeRequest.cpp" "REST Interface/NetworkRequestHandlers/CurrentTimeRequest.h" "REST Interface/NetworkRequestHandlers/AvailableDataTypesRequest.cpp" "REST Interface/NetworkRequestHandlers/AvailableDataTypesRequest.h" "REST Interface/NetworkRequestHandlers/GetDataRequest.cpp" "REST Interface/NetworkRequestHandlers/GetDataRequest.h" "REST Interface/NetworkRequestHandlers/all.h" "REST Interface/NetworkRequestHandlers/GetStatePeriodsRequest.cpp" "REST Interface/NetworkRequestHandlers/GetStatePeriodsRequest.h" "REST Interface/Model/stateTypes.h" "REST Interface/NetworkRequestHandlers/AddStatePeriodRequest.cpp" "REST Interface/NetworkRequestHandlers/AddStatePeriodRequest.h" "REST Interface/Conversions.cpp" "REST Interface/Conversions.h")

    target_link_libraries(Cpp_Middleware PRIVATE libsqlite3.dylib spdlog::spdlog)
    include_directories(/usr/local/Cellar/doctest/2.4.0/include /usr/local/Cellar/range-v3/0.11.0/include /usr/local/Cellar/boost/1.74.0 /usr/local/opt/sqlite/include)
ENDIF ()
