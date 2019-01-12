#include "mqtt.h"
void MQTT_mosquitto::my_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
    puts("void my_message_callback()");
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

void MQTT_mosquitto::my_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
    puts("my_connect_callback()");
    int i;
    if(!result){
        /* Subscribe to broker information topics on successful connect. */
        mosquitto_subscribe(mosq, NULL, "iDom/#", 2);
    }else{
        fprintf(stderr, "Connect failed\n");
    }
}

void MQTT_mosquitto::my_subscribe_callback(struct mosquitto *mosq, void *userdata, int mid, int qos_count, const int *granted_qos)
{
    puts("my_subscribe_callback()");
    int i;

    printf("Subscribed (mid: %d): %d", mid, granted_qos[0]);
    for(i=1; i<qos_count; i++){
        printf(", %d", granted_qos[i]);
    }
    printf("\n");
}

void MQTT_mosquitto::my_log_callback(struct mosquitto *mosq, void *userdata, int level, const char *str)
{
    puts("my_log_callback()");
    /* Pring all log messages regardless of level. */
    printf("%s\n", str);
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
    mosquitto_publish(_mosq, NULL, topic.c_str(), msg.size(), msg.c_str(), qos, false);
    puts("MQTT_mosquitto::publish() end");
}

void MQTT_mosquitto::subscribe(const std::string &topic)
{
    mosquitto_loop_forever(_mosq, -1, 1);
}

void MQTT_mosquitto::disconnect()
{
    puts("MQTT_mosquitto::disconnect() start");
    mosquitto_disconnect(_mosq);

    puts("MQTT_mosquitto::disconnect() end");
}
