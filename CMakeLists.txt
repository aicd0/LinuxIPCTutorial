cmake_minimum_required(VERSION 3.14)

set(MAIN_PROJECT_NAME ipc_demo)
project(${MAIN_PROJECT_NAME} VERSION 0.1.0)

set(MAIN_OUTPUT_DIR ${CMAKE_BINARY_DIR}/bin)

add_subdirectory(shared)
add_subdirectory(1_pipe)
add_subdirectory(2_fifo)
add_subdirectory(3_mmap)
add_subdirectory(4_shm)
add_subdirectory(5_msg)
add_subdirectory(6_sem)
add_subdirectory(7_signal)
add_subdirectory(8_socket)
