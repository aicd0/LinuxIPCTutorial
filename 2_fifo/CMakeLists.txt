project(${MAIN_PROJECT_NAME}_fifo VERSION 0.1.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MAIN_OUTPUT_DIR}/${PROJECT_NAME})
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")

add_executable(sender
  sender.cpp
  utils.cpp
)

target_include_directories(sender
  PRIVATE ${PROJECT_SOURCE_DIR},
)

add_executable(receiver
  receiver.cpp
  utils.cpp
)

target_include_directories(receiver
  PRIVATE ${PROJECT_SOURCE_DIR},
)
