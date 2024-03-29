#ifndef MQTT_MOSQUITTO
#define MQTT_MOSQUITTO

#include <mosquitto.h>
#include <string>
#include <functional>
#include <mutex>
#include <queue>

class MQTT_mosquitto
{
    std::string _topic = "NULL";
    std::string _host = "NULL";
    int _port = 0;
    int _qos = 2;
    int _keepalive = 60;
    bool _clean_session = true;
    struct mosquitto *_mosq = nullptr;
    bool _debugeMode = false;
    std::queue<std::pair<std::string,std::string>> _receivQueue;

protected:
    std::mutex _publish_mutex;
    std::mutex _queue_mutex;

public:
    bool _subscribed = false;
    bool _connected = false;

    explicit MQTT_mosquitto(const std::string& username,
                   bool clean_session = true);

    ~MQTT_mosquitto();

    /////////// This must be called before calling connect()
    int setLoginAndPass(const std::string& username, const std::string& pass);

    void connect(const std::string& topic,
                 const std::string& host = "localhost",
                 int port = 1883,
                 int qos = 2,
                 int keepalive = 60);

    int publish(const std::string& topic, const std::string& msg, int qos = 2, bool retained = false);
    int publishRetained(const std::string& topic, const std::string& msg, int qos = 2);
    void disconnect();
    void turnOnDebugeMode();
    void turnOffDebugeMode();

    static void subscribeHandlerRunInThread(MQTT_mosquitto* ptrMQTT);

    void putToReceiveQueue(const std::string& topic, const std::string& msg);
    int getReceiveQueueSize();
    std::pair<std::string, std::string> getMessage();
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
