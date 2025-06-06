cmake_minimum_required(VERSION 3.8)

project(test-dut)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall")

# === A6000專用新增的兩行 ===
set(SYSTEMC_HOME /usr/local/systemc-2.3.3)
include_directories(${SYSTEMC_HOME}/include)
link_directories(${SYSTEMC_HOME}/lib-linux64)

file(GLOB SRC "*.cpp")
add_executable(test-dut ${SRC})
target_link_libraries(test-dut systemc)
