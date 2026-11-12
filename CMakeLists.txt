
cmake_minimum_required(VERSION 3.20)

project(packet-analyzer
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(PkgConfig REQUIRED)
pkg_check_modules(PCAP REQUIRED IMPORTED_TARGET libpcap)

add_executable(packet-analyzer
    src/main.cpp
    src/capture/PacketCapture.cpp
)

target_include_directories(packet-analyzer
    PRIVATE include
)

target_link_libraries(packet-analyzer
    PRIVATE PkgConfig::PCAP
)

target_compile_options(packet-analyzer PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

