project(${MAIN_PROJECT_NAME}_socket VERSION 0.1.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MAIN_OUTPUT_DIR}/${PROJECT_NAME})
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")

include_directories(
  PRIVATE ${PROJECT_SOURCE_DIR}
)

add_executable(socket_server
  server.cpp buffer_reader.cpp shared.cpp
)

add_executable(socket_client
  client.cpp buffer_reader.cpp shared.cpp
)
