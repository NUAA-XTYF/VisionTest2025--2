cmake_minimum_required(VERSION 3.10.0)

project(RobotSimulation VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MinGW)
    add_compile_options(-Wa,-mbig-obj)
endif()

add_subdirectory(yaml-cpp)

set(SRCFILES
    src/main.cpp
    src/VisionTest.cpp)
#添加源文件目录
include_directories(${CMAKE_CURRENT_SOURCE_DIR}
    includes)

add_executable(RobotSimulation ${SRCFILES})
#在SRCFILES文件夹中添加可执行文件
target_include_directories(RobotSimulation
  PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
target_include_directories(RobotSimulation
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)
#为VisionTest指定包含目标目录，头文件路径为当前CMakeLists.txt所在目录的src文件夹
target_link_libraries(RobotSimulation PRIVATE yaml-cpp::yaml-cpp)