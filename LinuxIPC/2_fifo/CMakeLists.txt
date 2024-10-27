project(${MAIN_PROJECT_NAME}_fifo VERSION 0.1.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MAIN_OUTPUT_DIR}/${PROJECT_NAME})
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")

include_directories(
  PRIVATE ${PROJECT_SOURCE_DIR},
)

add_executable(fifo_sender
  sender.cpp
  utils.cpp
)

add_executable(fifo_receiver
  receiver.cpp
  utils.cpp
)
