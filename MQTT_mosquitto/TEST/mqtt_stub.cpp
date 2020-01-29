#include "../mqtt.h"
#include <iostream>
#include <unistd.h>



MQTT_mosquitto::MQTT_mosquitto(const std::string& username,
                               bool clean_session): _clean_session(clean_session)
{
    puts("MQTT_mosquitto::MQTT_mosquitto()");
}



void MQTT_mosquitto::connect(const std::string& topic,
                             const std::string& host,
                             int port,
                             int qos,
                             int keepalive)
{

}

MQTT_mosquitto::~MQTT_mosquitto()
{
    puts("MQTT_mosquitto::~MQTT_mosquitto()");
}

int MQTT_mosquitto::setLoginAndPass(const std::string &username, const std::string &pass)
{
        return 0;
}

int MQTT_mosquitto::publish(const std::string &topic, const std::string &msg, int qos, bool retained)
{
    std::cout << "MQTT_mosquitto::publish topic: " << topic << " msg: " << msg << std::endl;
    return 0;
}

int MQTT_mosquitto::publishRetained(const std::string &topic, const std::string &msg, int qos)
{
    std::cout << "MQTT_mosquitto::publishRatained topic: " << topic << " msg: " << msg << std::endl;
    return 0;
}

void MQTT_mosquitto::disconnect()
{

}

void MQTT_mosquitto::turnOnDebugeMode()
{
    _debugeMode = true;
    puts("MQTT debuge monde ON");
}

void MQTT_mosquitto::turnOffDebugeMode()
{
    _debugeMode = false;
    puts("MQTT debuge mode OFF");
}

void MQTT_mosquitto::putToReceiveQueue(const std::string &topic, const std::string &msg)
{

}

int MQTT_mosquitto::getReceiveQueueSize()
{
    return 0;
}

std::pair<std::string, std::string> MQTT_mosquitto::getMessage()
{
    return std::make_pair("null","test");
}

void MQTT_mosquitto::subscribeHandlerRunInThread(MQTT_mosquitto* ptrMQTT)
{

}
