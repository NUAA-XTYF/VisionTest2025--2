cmake_minimum_required(VERSION 3.10)
project(VisiSimulation)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找yaml-cpp
find_package(yaml-cpp REQUIRED)

# 包含目录
include_directories(include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party)

# 源文件
set(SOURCES
    src/main.cpp
    src/object.cpp
    src/target.cpp
    src/armor_plate.cpp
    src/energy_mechanism.cpp
    src/obstacle.cpp
    src/robot.cpp
    src/enemy_robot.cpp
    src/sentry_robot.cpp
    src/geometry.cpp
    src/config.cpp
   
)

# 可执行文件
add_executable(visi_sim ${SOURCES})

# 链接库
target_link_libraries(visi_sim yaml-cpp)

# 复制配置文件
file(COPY config DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
