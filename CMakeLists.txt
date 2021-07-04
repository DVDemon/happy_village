cmake_minimum_required(VERSION 3.2)

project(happy_village_server C CXX)

SET (SERVER_BINARY "happy_village_server")


find_package(OpenSSL)
find_package(GTest REQUIRED)
find_package(Threads)
find_package(ZLIB)
find_package(Poco REQUIRED COMPONENTS Foundation Util Net XML JSON Crypto NetSSL)
find_package(CppKafka REQUIRED)
find_package(Boost COMPONENTS filesystem system program_options regex REQUIRED)
find_package(OpenCV)

if(NOT ${Poco_FOUND})
    message(FATAL_ERROR "Poco C++ Libraries not found.")
endif()

include_directories(${Poco_INCLUDE_DIRS})
include_directories(${GTEST_INCLUDE_DIRS})
include_directories("/usr/include/mysql")
include_directories("/usr/local/include/mysql")
include_directories(${Boost_INCLUDE_DIR})
include_directories(${OpenCV_INCLUDE_DIR})
link_directories("/usr/local/lib")

add_executable(opencv_test opencv_test.cpp)

add_executable(${SERVER_BINARY} main.cpp 
                                config/config.cpp
                                database/database.cpp
                                database/cache.cpp
                                database/person.cpp
                                capture/capture_service.cpp)

target_include_directories(${SERVER_BINARY} PRIVATE "${CMAKE_BINARY_DIR}")
target_compile_options(${SERVER_BINARY} PRIVATE -Wall -Wextra -pedantic -Werror )

target_link_libraries(opencv_test ${OpenCV_LIBRARIES})

target_link_libraries(${SERVER_BINARY} PRIVATE 
                             ${CMAKE_THREAD_LIBS_INIT} 
                             ${Poco_LIBRARIES}
                             "libPocoData.so"
                             "libPocoDataMySQL.so"
                             "libmysqlclient.so"
                             "libignite-binary.so"
                             "libignite-common.so"
                             "libignite-thin-client.so"                            
                             "rdkafka++"
                             CppKafka::cppkafka
                             ZLIB::ZLIB
                             ${Boost_LIBRARIES}
                             ${OpenCV_LIBRARIES})

set_target_properties(${SERVER_BINARY} PROPERTIES LINKER_LANGUAGE CXX)
set_target_properties(${SERVER_BINARY} PROPERTIES CXX_STANDARD 17 CXX_STANDARD_REQUIRED ON)

install(TARGETS ${SERVER_BINARY} RUNTIME DESTINATION bin)

set(CPACK_GENERATOR DEB)
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_PACKAGE_CONTACT ddzuba@yandex.ru)
include(CPack)