project(${MAIN_PROJECT_NAME}_shared VERSION 0.1.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MAIN_OUTPUT_DIR}/${PROJECT_NAME})
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")

add_library(shared STATIC
  shared.cpp
)

target_include_directories(shared
  PRIVATE ${PROJECT_SOURCE_DIR},
)
