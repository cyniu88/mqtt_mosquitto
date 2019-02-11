
#include <iostream>
#include <string>
#include <thread>
#include "../../MQTT_mosquitto/mqtt.h"

std::string host = "test.mosquitto.org";//"192.168.4.1";
std::string topic = "#";
std::string msg = "hello";

int main()
{

    MQTT_mosquitto mqtt_server("integration_sut");

    mqtt_server.turnOffDebugeMode();

    mqtt_server.connect(topic,host);

    puts("odpalmy subscribera MQTT w nowym watku");
    auto th1 = std::thread(MQTT_mosquitto::subscribeHandlerRunInThread,&mqtt_server);
    th1.detach();

    while(mqtt_server._subscribed == false){
    }
std::cout << "Hello World!" << std::endl;
    while(true){

//        getline(std::cin, msg);

//        if(msg == "EXIT"){
//            break;
//        }

//        mqtt_server.publish("iDom/pub",msg,2);
        if (mqtt_server.getReceiveQueueSize() > 0){
            auto msg = mqtt_server.getMessage();
            std::cout << "t: " << msg.first << " " << msg.second << std::endl;
        }
    }
    std::cout << "Hello World!" << std::endl;
    return 0;
}

