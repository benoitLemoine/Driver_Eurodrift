cmake_minimum_required(VERSION 3.10)
project(Driver_Eurodrift)
set(CMAKE_C_STANDARD 99)


set(CMAKE_BINARY_DIR  ${CMAKE_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

add_executable(${PROJECT_NAME} src/main.c src/mapEditing.c)

target_link_libraries(${PROJECT_NAME} m)