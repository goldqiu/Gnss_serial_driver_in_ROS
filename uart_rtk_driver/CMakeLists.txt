cmake_minimum_required(VERSION 3.0.2)
project(uart_rtk_driver)

SET(CMAKE_BUILD_TYPE "Release")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")
add_compile_options(-std=c++14)
add_definitions(-std=c++14)

find_package(catkin REQUIRED COMPONENTS
  rospy
  roscpp
  std_msgs
  nav_msgs
  message_generation 

)

find_package(PkgConfig REQUIRED)
pkg_check_modules(YAML_CPP REQUIRED yaml-cpp)
set(WORK_SPACE_PATH ${PROJECT_SOURCE_DIR})
configure_file (
  ${PROJECT_SOURCE_DIR}/include/global_defination/global_defination.h.in
  ${PROJECT_BINARY_DIR}/include/global_defination/global_defination.h)


add_message_files(
    DIRECTORY msg
    FILES
    rtk.msg
  )
generate_messages(
     DEPENDENCIES
     std_msgs
  ) 

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES uart_rtk_driver
#  CATKIN_DEPENDS rospy std_msgs
#  DEPENDS system_lib
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/src
  ${YAML_CPP_INCLUDEDIR}
  ${PROJECT_BINARY_DIR}/include
)
file(GLOB_RECURSE ALL_SRCS "*.cc")


add_executable(rtk_driver_node src/rtk_driver/rtk_read.cpp ${ALL_SRCS})
target_link_libraries(rtk_driver_node ${catkin_LIBRARIES}  ${YAML_CPP_LIBRARIES})


