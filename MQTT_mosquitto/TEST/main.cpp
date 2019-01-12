#include <stdio.h>
#include <mosquitto.h>
#include <string>
#include <thread>
#include <iostream>
#include "../mqtt.h"

int main(int argc, char *argv[])
{
    int ilosc = 30;
    /// iot.eclipse.org
    MQTT_mosquitto mq("test_iDomServer","192.168.4.1",1883,60,false);
    mq.turnOffDebugeMode();

    puts("odlapalmy subscribera MQTT w nowym watku");
    auto th1 = std::thread(MQTT_mosquitto::subscribeHandlerRunInThread,&mq, "iDom/#" , 2);
    th1.detach();

    for(int i = 1; i < ilosc+1 ; ++ i)
    {
        mq.publish("iDom/pies", "test message: " + std::to_string(i),0);
        std::cout << "wysłano wiadomosc numer: " << i << std::endl;
    }
    std::this_thread::sleep_for(std::chrono::seconds(ilosc/3));
    int ssize = MQTT_mosquitto::getReceiveQueueSize();
    if( ssize != ilosc){
        std::cout <<"error  nie odebralem wszystkich wiadomosci " << ssize << std::endl;
        return 1;
    }
    else {
        std::cout << "gra ilosc wiadomosci odebranych " << ssize << std::endl;
    }

    for(int i = 1; i < ssize+1; ++i){
        std::cout << "w forze: " << i << std::endl;
        auto pairMSG = MQTT_mosquitto::getMessage();
        if(pairMSG.second == "test message: " + std::to_string(i)){
            std::cout << "jest ok porownanie: " << i << std::endl;
        }
        else {
            std::cout << "a to nie jest ok porownanie: " << i << std::endl;
            return 1;
        }
    }
    mq.disconnect();

    return 0;
}
