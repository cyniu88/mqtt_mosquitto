cmake_minimum_required(VERSION 2.8)

project(MQTT_mosquito_TEST)
set(SOURCE_FILES  "main.cpp"
    "../mqtt.h"
    "../mqtt.cpp"
    )
add_executable(${PROJECT_NAME} ${SOURCE_FILES} )
TARGET_LINK_LIBRARIES( ${PROJECT_NAME} mosquitto pthread)
