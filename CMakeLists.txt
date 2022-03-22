cmake_minimum_required(VERSION 3.14)

set(MAIN_PROJECT_NAME ipc_demo)
project(${MAIN_PROJECT_NAME} VERSION 0.1.0)

set(MAIN_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(pipe)
