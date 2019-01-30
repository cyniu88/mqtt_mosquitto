#ifndef MQTT_MOSQUITTO
#define MQTT_MOSQUITTO

#include <mosquitto.h>
#include <string>
#include <functional>
#include <mutex>
#include <queue>

struct MQTT_callback_DATA{
    std::string topic = "NULL";
    bool subscribed = false;
};

class MQTT_mosquitto
{
public:
    MQTT_callback_DATA callbackData;
    std::string _host;
    int _port;
    int _keepalive;
    bool _clean_session;
    struct mosquitto *_mosq = NULL;
    static bool _debugeMode;
    static std::queue<std::pair<std::string,std::string>> _receivQueue;

protected:
    static std::mutex _publish_mutex;
    static std::mutex _queue_mutex;
public:
    MQTT_mosquitto(const std::string& username,
                   const std::string& topic,
                   const std::string& host = "localhost",
                   int port = 1883,
                   int keepalive = 60,
                   bool clean_session = true);
    ~MQTT_mosquitto();

    int publish(const std::string&  topic, const std::string& msg, int qos);
    void disconnect();
    void turnOnDebugeMode();
    void turnOffDebugeMode();

    static void subscribeHandlerRunInThread(MQTT_mosquitto* ptrMQTT);

    static void putToReceiveQueue(const std::string& topic, const std::string& msg);

    static int getReceiveQueueSize();
    static std::pair<std::string, std::string> getMessage();
private:
    //callback
    static void my_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message);
    static void my_connect_callback(struct mosquitto *mosq, void *userdata, int result);
    static void my_disconnect_callback(struct mosquitto *mosq, void *userdata, int result);
    static void my_subscribe_callback(struct mosquitto *mosq, void *userdata, int mid,
                                      int qos_count, const int *granted_qos);
    static void my_unsubscribe_callback(struct mosquitto *mosq, void *userdata, int result);
    static void my_log_callback(struct mosquitto *mosq, void *userdata, int level, const char *str);
};

#endif
