project(${MAIN_PROJECT_NAME}_shared VERSION 0.1.0)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${MAIN_OUTPUT_DIR}/${PROJECT_NAME})
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")

include_directories(
  PRIVATE ${PROJECT_SOURCE_DIR}
)

# common
add_library(common STATIC
  common.cpp
)

# shm_manager
add_library(shm_manager STATIC
  shm_manager.cpp
)

# sem_manager
add_library(sem_manager STATIC
  sem_manager.cpp
)

target_link_libraries(sem_manager
  PRIVATE shm_manager
)
