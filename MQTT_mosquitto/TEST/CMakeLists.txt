cmake_minimum_required(VERSION 2.8)
# Get the current working branch
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
# Get the latest abbreviated commit hash of the working branch
execute_process(
  COMMAND git rev-parse HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_COMMIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

add_definitions(-DGIT_COMMIT_HASH="${GIT_COMMIT_HASH}")
add_definitions(-DGIT_BRANCH="${GIT_BRANCH}")
message("dane :")
message(${GIT_COMMIT_HASH})

project(MQTT_mosquito_TEST)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(SOURCE_FILES  "main.cpp"
    "../mqtt.h"
    "../mqtt.cpp"
    )
add_executable(${PROJECT_NAME} ${SOURCE_FILES} )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} mosquitto pthread gtest)
