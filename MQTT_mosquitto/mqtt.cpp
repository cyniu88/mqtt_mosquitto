#include "mqtt.h"
#include <iostream>
#include <unistd.h>

void MQTT_mosquitto::my_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
    auto mqttPTR = static_cast<MQTT_mosquitto*>(userdata);
    if(mqttPTR->_debugeMode == true)
    {
        puts("void my_message_callback()");
        puts("\n\nodebralem:");

        if(message->payloadlen){
            std::cout << "topic: " << message->topic << " msg: " << static_cast<char*>(message->payload) << "|" << std::endl;
        }
        else
        {
            std::cout << "topic: " << message->topic << std::endl;
        }
        fflush(stdout);

        //        std::string rec = static_cast<char*>(message->payload);
        //        if(rec == "EXIT")
        //        {
        //            puts("EXIT JEST");
        //            mosquitto_disconnect(mosq);
        //        }
    }

    std::string m;
    if(message->payloadlen){
        m = static_cast<char*>(message->payload);
    }

    mqttPTR->putToReceiveQueue(message->topic, m);
}

void MQTT_mosquitto::my_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
    auto mqttPTR = static_cast<MQTT_mosquitto*>(userdata);
    mqttPTR->_connected = true;
    mosquitto_subscribe(mosq,nullptr, mqttPTR->_topic.c_str(),2);

    if(mqttPTR->_debugeMode == true)
    {
        puts("my_connect_callback()");
        if(!result){
            puts("connectd to broker");
        }
        else
        {
            std::cout << stderr << " Connect failed" << std::endl;
        }
    }
}

void MQTT_mosquitto::my_disconnect_callback(mosquitto *mosq, void *userdata, int result)
{
    auto mqttPTR = static_cast<MQTT_mosquitto*>(userdata);
    mqttPTR->_subscribed = false;
    mqttPTR->_connected = false;

    if(mqttPTR->_debugeMode == true)
    {
        std::cout << "my_disconnect_callback()" << std::endl <<" disconnect result " << result << std::endl;

        if(result == 0){
            puts("disconnectd to broker");
        }
        else
        {
            std::cout << stderr << " Disconnect failed" << std::endl;
        }
    }
}

void MQTT_mosquitto::my_subscribe_callback(struct mosquitto *mosq,
                                           void *userdata,
                                           int mid,
                                           int qos_count,
                                           const int *granted_qos)
{
    auto mqttPTR = static_cast<MQTT_mosquitto*>(userdata);
    mqttPTR->_subscribed = true;

    if(mqttPTR->_debugeMode == true)
    {
        puts("my_subscribe_callback()");
        int i;

        std::cout <<"Subscribed (mid: " << mid << "): " << ", granted_qos: " << granted_qos[0] << std::endl;
        for(i=1; i<qos_count; i++){
            std::cout << ", " << granted_qos[i] << " " ;
        }
        std::cout << std::endl;
    }
}

void MQTT_mosquitto::my_unsubscribe_callback(mosquitto *mosq, void *userdata, int result)
{
    auto mqttPTR = static_cast<MQTT_mosquitto*>(userdata);
    mqttPTR->_subscribed = false;
    if(mqttPTR->_debugeMode == true)
    {
        puts("my_unsubscribe_callback()");
        puts("Unubscribed");
    }
}

void MQTT_mosquitto::my_log_callback(struct mosquitto *mosq,
                                     void *userdata,
                                     int level,
                                     const char *str)
{
    auto mqttPTR = static_cast<MQTT_mosquitto*>(userdata);
    if(mqttPTR->_debugeMode == true)
    {
        puts("my_log_callback()");
        puts(str);
    }
}

MQTT_mosquitto::MQTT_mosquitto(const std::string& username,
                               bool clean_session): _clean_session(clean_session)
{
    mosquitto_lib_init();
    _mosq = mosquitto_new(username.c_str(), clean_session, this);

    if(!_mosq){
        std::cout << stderr << " Error: Out of memory." << std::endl;
        throw "cannot create MQTT";
    }
    mosquitto_log_callback_set(_mosq, my_log_callback);
    mosquitto_connect_callback_set(_mosq, my_connect_callback);
    mosquitto_disconnect_callback_set(_mosq, my_disconnect_callback);
    mosquitto_message_callback_set(_mosq, my_message_callback);
    mosquitto_subscribe_callback_set(_mosq, my_subscribe_callback);
    mosquitto_unsubscribe_callback_set(_mosq, my_unsubscribe_callback);
}



void MQTT_mosquitto::connect(const std::string& topic,
                             const std::string& host,
                             int port,
                             int qos,
                             int keepalive)
{
    _topic = topic;
    _host = host;
    _port = port;
    _qos = qos;
    _keepalive = keepalive;

    mosquitto_threaded_set(_mosq, true);
    if(mosquitto_connect(_mosq, _host.c_str(), _port, _keepalive)){
        std::cout << stderr << " Unable to connect." << std::endl;
        throw std::string("cannot connect to MQTT broker");
    }
}

MQTT_mosquitto::~MQTT_mosquitto()
{
    disconnect();
    mosquitto_destroy(_mosq);
    mosquitto_lib_cleanup();
}

int MQTT_mosquitto::setLoginAndPass(const std::string &username, const std::string &pass)
{
    return mosquitto_username_pw_set(_mosq,username.c_str(), pass.c_str());
}

int MQTT_mosquitto::publish(const std::string &topic, const std::string &msg, int qos)
{
    int r = -1;
    if(_connected == true)
    {
        std::lock_guard <std::mutex> lock(_publish_mutex);
        r = mosquitto_publish(_mosq, nullptr, topic.c_str(), msg.size(), msg.c_str(), qos, false);

        if (r != 0){
            throw std::string("problem with publish MQTT");
        }
    }
    //usleep(1); //NOTE wait 10 usek due to SIGILL
    return r;
}

void MQTT_mosquitto::disconnect()
{
    if(_connected == true)
        mosquitto_disconnect(_mosq);
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
    std::lock_guard <std::mutex> lock(_queue_mutex);
    auto p = std::make_pair(topic,msg);
    _receivQueue.push(p);
}

int MQTT_mosquitto::getReceiveQueueSize()
{
    std::lock_guard <std::mutex> lock(_queue_mutex);
    return static_cast<int>(_receivQueue.size());
}

std::pair<std::string, std::string> MQTT_mosquitto::getMessage()
{
    std::lock_guard <std::mutex> lock(_queue_mutex);
    if (_receivQueue.empty() == true)
        throw std::string("Queue is empty");
    auto _return = _receivQueue.front();
    _receivQueue.pop();
    return _return;
}

void MQTT_mosquitto::subscribeHandlerRunInThread(MQTT_mosquitto* ptrMQTT)
{
    auto ret = mosquitto_loop_forever(ptrMQTT->_mosq, -1, 1);
    std::cout << "loop forever return: " << ret << std::endl;
}
