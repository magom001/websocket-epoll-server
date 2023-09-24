cmake_minimum_required(VERSION 3.0.0)
project(sockets VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenSSL REQUIRED)

include(CTest)
enable_testing()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
add_executable(sockets main.cpp picohttpparser.cpp base64.cpp crypto.cpp websocket_utility.cpp)

target_include_directories(sockets PRIVATE OPENSSL_INCLUDE_DIR)
target_link_libraries(sockets PRIVATE OpenSSL::applink crypto ssl)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
