cmake_minimum_required(VERSION 2.8)

project(MQTT_integration)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(SOURCE_FILES  "main.cpp"
    "../../MQTT_mosquitto/mqtt.h"
    "../../MQTT_mosquitto/mqtt.cpp"
    )
add_executable(${PROJECT_NAME} ${SOURCE_FILES} )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} mosquitto pthread gtest)
