#include <stdio.h>
#include <string>
#include <thread>
#include <iostream>
#include "../mqtt.h"
#include <gtest/gtest.h>

class MQTT_BT : public ::testing::Test
{
public:
    std::string topicSubscribe = "iDom/#";
    std::string topicPublish = "iDom/bt";
    std::string host = "localhost";
};

TEST_F(MQTT_BT, wrong_host)
{
    EXPECT_ANY_THROW(MQTT_mosquitto("test_iDomServer","fake host",1883,60,false));
}

TEST_F(MQTT_BT, flow)
{
    int ilosc = 8;
    /// iot.eclipse.org
    MQTT_mosquitto mq("test_iDomServer",host,1883,60,false);
    mq.turnOnDebugeMode();

    puts("odpalmy subscribera MQTT w nowym watku");
    auto th1 = std::thread(MQTT_mosquitto::subscribeHandlerRunInThread,&mq, topicSubscribe.c_str() , 2);
    th1.detach();

    while(MQTT_mosquitto::_subscribed == false){
    }


    for(int i = 1; i < ilosc+1 ; ++ i)
    {
        mq.publish(topicPublish, "test message: " + std::to_string(i),0);
        std::cout << "wysłano wiadomosc numer: " << i << std::endl;
    }
    puts("start sleep");
    std::this_thread::sleep_for(std::chrono::seconds(10));
    puts("stop sleep");
    int ssize = MQTT_mosquitto::getReceiveQueueSize();
    if( ssize != ilosc){
        FAIL() <<"error  nie odebralem wszystkich wiadomosci " << ssize << std::endl;
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
            FAIL() << "a to nie jest ok porownanie: " << i << std::endl;
        }
    }
    mq.disconnect();

}
int main(int argc, char *argv[])
{

    ::testing::InitGoogleTest( &argc, argv );
    //::testing::GTEST_FLAG(filter) = "functions_fixture.sleepThread";
    //::testing::GTEST_FLAG(filter) = "event_counter_fixture.getLast1minNumberEvent";
    //::testing::GTEST_FLAG(filter) = "command433MHz_Class_fixture.*)";
    auto ret = RUN_ALL_TESTS();

    const char * PROG_INFO = " Basic TEST iDomServer: " __DATE__ ", " __TIME__;
    std::cout << "wersja " <<PROG_INFO <<" "<< GIT_BRANCH <<" " << GIT_COMMIT_HASH << std::endl;
    return ret;
}
