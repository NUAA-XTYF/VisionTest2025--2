#
cmake_minimum_required(VERSION 3.10)
project(VisionTest2)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
find_package(yaml-cpp CONFIG REQUIRED)

# 将源代码添加到此项目的可执行文件。
add_executable (VisionTest2 "VisionTest2.cpp" "VisionTest2.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET VisionTest2 PROPERTY CXX_STANDARD 20)
endif()


target_link_libraries(VisionTest2 PRIVATE yaml-cpp::yaml-cpp)
target_include_directories(VisionTest2 PRIVATE .\\package)


