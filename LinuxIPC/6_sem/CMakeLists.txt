project(${MAIN_PROJECT_NAME}_sem VERSION 0.1.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MAIN_OUTPUT_DIR}/${PROJECT_NAME})
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")

link_libraries(
  common
  sem_manager
)

include_directories(
  PRIVATE ${PROJECT_SOURCE_DIR},
  PRIVATE "../shared"
)

add_executable(sem
  sem.cpp
)
