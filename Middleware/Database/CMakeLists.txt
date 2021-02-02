file(GLOB Database_SRC CONFIGURE_DEPENDS "*.cpp")

add_library(Database ${Database_SRC} IDatabaseFactory.h DatabaseFactory.cpp DatabaseFactory.h IStatistics.h IConfigurationAccess.h IDataAccess.h IStateStorage.h IAlarmStorage.h DataStorage.cpp DataStorage.h StateStorage.cpp StateStorage.h StatisticsStorage.cpp StatisticsStorage.h AlarmStorage.cpp AlarmStorage.h)

if (WIN32)
    add_compile_definitions(NOMINMAX)
    add_definitions(-D_WIN32_WINNT=0x0A00)
endif ()

target_include_directories(Database PRIVATE
        ${CMAKE_SOURCE_DIR}/Types
        )
