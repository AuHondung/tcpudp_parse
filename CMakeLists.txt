# CMake 的最低版本要求
cmake_minimum_required(VERSION 3.10)

# 项目名称
project(18_TCPUDP_DEMO)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 可执行文件输出的名称和源文件
add_executable(udp_client udp_client.cpp)
add_executable(udp_server udp_server.cpp)
add_executable(tcp_parse tcp_parse.cpp)
add_executable(pcap_udp pcap_udp.cpp)

# find_package(Pcap REQUIRED)
target_link_libraries(pcap_udp pcap)