project(${MAIN_PROJECT_NAME}_msg VERSION 0.1.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MAIN_OUTPUT_DIR}/${PROJECT_NAME})
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")

link_libraries(
  shared
)

include_directories(
  PRIVATE ${PROJECT_SOURCE_DIR},
  PRIVATE "../shared"
)

add_executable(msg_sender
  sender.cpp
)

add_executable(msg_receiver
  receiver.cpp
)
