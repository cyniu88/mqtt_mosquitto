#include "mqtt.h"
#include <iostream>
#include <unistd.h>

bool MQTT_mosquitto::_debugeMode = false;
bool MQTT_mosquitto::_subscribed = false;
void MQTT_mosquitto::my_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
    if(_debugeMode == true)
    {
        puts("void my_message_callback()");
        puts("\n\nodebralem:");
        if(message->payloadlen){
            printf("%s %s\n", message->topic, message->payload);
        }else{
            printf("%s (null)\n", message->topic);
        }
        fflush(stdout);

        std::string rec = (char*)(message->payload);
        if(rec == "EXIT")
        {
            puts("EXIT JEST");
            mosquitto_disconnect(mosq);
        }
    }
    putToReceiveQueue((char*)(message->topic),(char*)(message->payload));
}

void MQTT_mosquitto::my_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
    if(_debugeMode == true)
    {
        puts("my_connect_callback()");
        if(!result){
            /* Subscribe to broker information topics on successful connect. */
            //mosquitto_subscribe(mosq, NULL, "iDom/#", 2);
            puts("connectd to broker");
        }else{
            fprintf(stderr, "Connect failed\n");
        }
    }
}

void MQTT_mosquitto::my_subscribe_callback(struct mosquitto *mosq, void *userdata, int mid, int qos_count, const int *granted_qos)
{
    _subscribed = true;
    if(_debugeMode == true)
    {
        puts("my_subscribe_callback()");
        int i;

        printf("Subscribed (mid: %d): %d", mid, granted_qos[0]);
        for(i=1; i<qos_count; i++){
            printf(", %d", granted_qos[i]);
        }
        printf("\n");
    }
}

void MQTT_mosquitto::my_log_callback(struct mosquitto *mosq, void *userdata, int level, const char *str)
{
    if(_debugeMode == true)
    {
        puts("my_log_callback()");
        /* Pring all log messages regardless of level. */
        printf("%s\n", str);
    }
}

MQTT_mosquitto::MQTT_mosquitto(const std::string& username,
                               const std::string& host,
                               int port,
                               int keepalive,
                               bool clean_session): _host(host),
                                                     _port(port),
                                                     _keepalive(keepalive),
                                                     _clean_session(clean_session)
{
    puts("MQTT_mosquitto::MQTT_mosquitto() start");
    mosquitto_lib_init();
    _mosq = mosquitto_new(username.c_str(), clean_session, NULL);
    if(!_mosq){
        fprintf(stderr, "Error: Out of memory.\n");
        throw "cannot create MQTT";
    }
    mosquitto_log_callback_set(_mosq, my_log_callback);
    mosquitto_connect_callback_set(_mosq, my_connect_callback);
    mosquitto_message_callback_set(_mosq, my_message_callback);
    mosquitto_subscribe_callback_set(_mosq, my_subscribe_callback);

    if(mosquitto_connect(_mosq, _host.c_str(), _port, _keepalive)){
        fprintf(stderr, "Unable to connect.\n");
        throw "cannot connect to MQTT broker";
    }
    puts("MQTT_mosquitto::MQTT_mosquitto() end");
}

MQTT_mosquitto::~MQTT_mosquitto()
{
    puts("MQTT_mosquitto::~MQTT_mosquitto() start");
    mosquitto_destroy(_mosq);
    mosquitto_lib_cleanup();
    puts("MQTT_mosquitto::~MQTT_mosquitto() end");
}

int MQTT_mosquitto::publish(const std::string &topic, const std::string &msg, int qos)
{
    puts("MQTT_mosquitto::publish() start");
    std::lock_guard <std::mutex> lock(_publish_mutex);
    int r =  mosquitto_publish(_mosq, NULL, topic.c_str(), msg.size(), msg.c_str(), qos, false);
    if (r != 0){
        throw "problem with publish MQTT";
    }
    usleep(10); //NOTE wait 10 usek due to SIGILL
    return r;
}

void MQTT_mosquitto::subscribe(const std::string &topic, int qos)
{
    mosquitto_subscribe(_mosq,NULL, topic.c_str(), qos);
    mosquitto_loop_forever(_mosq, -1, 1);
}

void MQTT_mosquitto::disconnect()
{
    puts("MQTT_mosquitto::disconnect() start");
    mosquitto_disconnect(_mosq);

    puts("MQTT_mosquitto::disconnect() end");
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

std::mutex MQTT_mosquitto::_queue_mutex;
std::queue<std::pair<std::string,std::string>> MQTT_mosquitto::_receivQueue;

void MQTT_mosquitto::putToReceiveQueue(const std::string &topic, const std::string &msg)
{
    std::lock_guard <std::mutex> lock(_queue_mutex);
    auto p = std::make_pair(topic,msg);
    _receivQueue.push(p);
    //std::cout << " receive: topic - " << topic << " message - " <<msg << std::endl;
}

int MQTT_mosquitto::getReceiveQueueSize()
{
    std::lock_guard <std::mutex> lock(_queue_mutex);
    return _receivQueue.size();
}

std::pair<std::string, std::string> MQTT_mosquitto::getMessage()
{
    std::lock_guard <std::mutex> lock(_queue_mutex);
    if (_receivQueue.empty() == true)
        throw "Queue is empty";
    auto _return = _receivQueue.front();
    _receivQueue.pop();
    return _return;
}

void MQTT_mosquitto::subscribeHandlerRunInThread(MQTT_mosquitto* ptrMQTT, const std::string& topic, int qos)
{
    ptrMQTT->subscribe(topic.c_str(), qos);
}
