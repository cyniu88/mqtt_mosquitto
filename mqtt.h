#include <mosquitto.h>
#include <string>
#include <functional>
#include <mutex>

class MQTT_mosquitto
{
    std::string _host;
    int _port;
    int _keepalive;
    bool _clean_session;
    struct mosquitto *_mosq = NULL;

protected:
    std::mutex _publish_mutex;
public:
    MQTT_mosquitto(const std::string& username,
                   const std::string& host = "localhost",
                   int port = 1883,
                   int keepalive = 60,
                   bool clean_session = true);
    ~MQTT_mosquitto();

    int publish(const std::string&  topic, const std::string& msg, int qos);
    void run();
    void subscribe(const std::string& topic);
    void disconnect();
private:
    //callback
    static void my_message_callback(struct mosquitto *mosq, void *userdata, const struct mosquitto_message *message);
    static void my_connect_callback(struct mosquitto *mosq, void *userdata, int result);
    static void my_subscribe_callback(struct mosquitto *mosq, void *userdata, int mid,
                                      int qos_count, const int *granted_qos);
    static void my_log_callback(struct mosquitto *mosq, void *userdata, int level, const char *str);
};
