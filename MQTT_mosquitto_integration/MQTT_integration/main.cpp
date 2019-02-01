
#include <iostream>
#include <string>
#include <thread>
#include "../../MQTT_mosquitto/mqtt.h"

std::string host = "test.mosquitto.org";//"192.168.4.1";
std::string topic = "koko/#";
std::string msg = "hello";

int main()
{

    MQTT_mosquitto mqtt_server("integration_sut");

    mqtt_server.turnOnDebugeMode();

    mqtt_server.connect("iDom/#",host);

    puts("odpalmy subscribera MQTT w nowym watku");
    auto th1 = std::thread(MQTT_mosquitto::subscribeHandlerRunInThread,&mqtt_server);
    th1.detach();

    while(mqtt_server._subscribed == false){
    }

    while(true){

        getline(std::cin, msg);

        if(msg == "EXIT"){
            break;
        }

        mqtt_server.publish("iDom/pub",msg,2);
    }
    std::cout << "Hello World!" << std::endl;
    return 0;
}

