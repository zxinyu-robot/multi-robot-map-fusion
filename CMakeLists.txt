cmake_minimum_required(VERSION 3.20)
project(multi_robot_map_fusion VERSION 0.1.0 LANGUAGES CXX)

include(CTest)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(multi_robot_map_fusion
  src/shared_frame_slot.cpp
  src/submap_registry.cpp)
add_library(map_fusion::runtime ALIAS multi_robot_map_fusion)
target_include_directories(multi_robot_map_fusion PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  target_compile_options(multi_robot_map_fusion PRIVATE -Wall -Wextra -Wpedantic)
endif()
if(UNIX AND NOT APPLE)
  target_link_libraries(multi_robot_map_fusion PUBLIC rt)
endif()

add_executable(fusion_demo src/fusion_demo.cpp)
target_link_libraries(fusion_demo PRIVATE map_fusion::runtime)
if(BUILD_TESTING)
  add_executable(fusion_tests tests/fusion_tests.cpp)
  target_link_libraries(fusion_tests PRIVATE map_fusion::runtime)
  add_test(NAME fusion_tests COMMAND fusion_tests)
endif()
