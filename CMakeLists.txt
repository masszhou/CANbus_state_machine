cmake_minimum_required(VERSION 2.8.9)
set (CMAKE_CXX_STANDARD 11)

project (statemachine_example)

# search project root directory, save src files to DIR_SRCS
aux_source_directory(./src DIR_SRCS)
# target
add_executable(${PROJECT_NAME}  ${DIR_SRCS})