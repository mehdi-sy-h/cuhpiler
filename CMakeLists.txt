cmake_minimum_required(VERSION 3.30)

project(cuhpiler
  VERSION 0.1.0
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(
  -Wall
  -Wextra
  -Wpedantic
  -Wshadow
  -Wconversion
)

add_executable(${PROJECT_NAME}
  src/cuhpiler.cpp
)

target_link_options(${PROJECT_NAME} PRIVATE
  -fsanitize=address,undefined
)

target_compile_options(${PROJECT_NAME} PRIVATE
  $<$<CONFIG:Debug>:-g3 -Og -fsanitize=address,undefined>
  $<$<CONFIG:Release>:-O2 -DNDEBUG>
)

install(TARGETS ${PROJECT_NAME})
