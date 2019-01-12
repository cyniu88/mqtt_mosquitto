#include <stdio.h>
#include <mosquitto.h>
#include <string>
#include <thread>
#include <iostream>
#include "mqtt.h"
/*
void my_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message)
{
    puts("void my_message_callback()");
    if(message->payloadlen){
        printf("%s %s\n", message->topic, message->payload);
    }else{
        printf("%s (null)\n", message->topic);
    }
    fflush(stdout);

//    std::string rec = (char*)(message->payload);
//    if(rec == "EXIT")
//    {
//        puts("EXIT JEST");
//        mosquitto_disconnect(mosq);
//    }
}

void wyjdz(struct mosquitto *mosq){
    std::string k = "koko";
    do{
        std::cin >> k;
        if (k == "exit")
            break;
        else {
            mosquitto_publish(mosq, NULL, "iDom/sample", k.size(), k.c_str(), 0, false);
        }
    } while (true);
    puts("EXIT z klawiatury");
    mosquitto_disconnect(mosq);

}
void my_connect_callback(struct mosquitto *mosq, void *userdata, int result)
{
    puts("my_connect_callback()");
    int i;
    if(!result){
        // Subscribe to broker information topics on successful connect.
        mosquitto_subscribe(mosq, NULL, "iDom/#", 2);
    }else{
        fprintf(stderr, "Connect failed\n");
    }
}

void my_subscribe_callback(struct mosquitto *mosq, void *userdata, int mid, int qos_count, const int *granted_qos)
{
    puts("my_subscribe_callback()");
    int i;

    printf("Subscribed (mid: %d): %d", mid, granted_qos[0]);
    for(i=1; i<qos_count; i++){
        printf(", %d", granted_qos[i]);
    }
    printf("\n");
}

void my_log_callback(struct mosquitto *mosq, void *userdata, int level, const char *str)
{
    puts("my_log_callback()");
    // Pring all log messages regardless of level.
    printf("%s\n", str);
}
*/
int main(int argc, char *argv[])
{
            MQTT_mosquitto mq("test_iDomServer","192.168.4.1",1883,60,false);

           // mq.publish("iDom/pies", "wowwow",0);
            puts("konczymy");
            //auto th1 = std::thread( &mq.subscribe);
            mq.subscribe("iDom/#");
            std::string exit_code = "koko";

            do {
               std::cin >> exit_code;
            }
            while ( exit_code != "e");


            mq.disconnect();
         //   sleep(1);
//    const std::string& userName = "koko";
//    int i;
//    char *host = "192.168.4.1";
//    int port = 1883;
//    int keepalive = 60;
//    bool clean_session = true;
//    struct mosquitto *mosq = NULL;

//    mosquitto_lib_init();
//    mosq = mosquitto_new(userName.c_str(), clean_session, NULL);
//    if(!mosq){
//        fprintf(stderr, "Error: Out of memory.\n");
//        return 1;
//    }
//    mosquitto_log_callback_set(mosq, my_log_callback);
//    mosquitto_connect_callback_set(mosq, my_connect_callback);
//    mosquitto_message_callback_set(mosq, my_message_callback);
//    mosquitto_subscribe_callback_set(mosq, my_subscribe_callback);

//    if(mosquitto_connect(mosq, host, port, keepalive)){
//        fprintf(stderr, "Unable to connect.\n");
//        return 1;
//    }

//    auto th1 = std::thread(mosquitto_loop_forever,mosq, -1, 1);
//    auto th2 = std::thread(wyjdz,mosq);
//    th1.join();
//    th2.join();
//    mosquitto_destroy(mosq);
//    mosquitto_lib_cleanup();
    return 0;
}
