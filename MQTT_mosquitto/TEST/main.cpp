﻿#include <stdio.h>
#include <string>
#include <thread>
#include <iostream>
#include "../mqtt.h"
#include <gtest/gtest.h>

class MQTT_BT : public ::testing::Test
{
public:
    std::string topicSubscribe = "iDom/#";
    std::string topicPublish = "iDom/bt/cyniu";
    std::string host = "test.mosquitto.org";
};

TEST_F(MQTT_BT, emptyQueue)
{
    MQTT_mosquitto mq("test_iDomServer");
    try {
        mq.getMessage();
    } catch (const std::string& e) {
        std::cout << "wyjatek: " << e << std::endl;
        SUCCEED();
        return;
    }
    FAIL();
}

TEST_F(MQTT_BT, wrong_host)
{
    MQTT_mosquitto mq("test_iDomServer");
    EXPECT_ANY_THROW(mq.connect("fake_topic","fake host",1883,60,false));
}

TEST_F(MQTT_BT, publish_empty_string_and_get)
{
    int ilosc = 8;
    MQTT_mosquitto mq("test_iDomServer");
    mq.connect(topicSubscribe,host,1883,60,false);
    mq.turnOnDebugeMode();

    puts("odpalmy subscribera MQTT w nowym watku");
    auto th1 = std::thread(MQTT_mosquitto::subscribeHandlerRunInThread,&mq);
    th1.detach();

    while(mq._subscribed == false){
    }

    for(int i = 1; i < ilosc+1 ; ++ i)
    {
        mq.publish(topicPublish, "",0);
        std::cout << "wysłano wiadomosc numer: " << i << std::endl;
    }
    puts("start sleep");
    std::this_thread::sleep_for(std::chrono::seconds(7));
    puts("stop sleep");
    int ssize = mq.getReceiveQueueSize();
    if( ssize != ilosc){
        FAIL() <<"error nie odebralem wszystkich wiadomosci " << ssize << std::endl;
    }
    else {
        std::cout << "gra ilosc wiadomosci odebranych " << ssize << std::endl;
    }

    for(int i = 1; i < ssize+1; ++i){
        std::cout << "w forze: " << i << std::endl;
        auto pairMSG = mq.getMessage();
        if(pairMSG.second == ""){
            std::cout << "jest ok porownanie: " << i << std::endl;
        }
        else {
            FAIL() << "a to nie jest ok porownanie: " << i << std::endl;
        }
    }

    mq.disconnect();
}

TEST_F(MQTT_BT, flow)
{
    int ilosc = 80;
    MQTT_mosquitto mq("test_iDomServer");
    mq.connect(topicSubscribe,host,1883,60,false);
    mq.turnOnDebugeMode();

    puts("odpalmy subscribera MQTT w nowym watku");
    auto th1 = std::thread(MQTT_mosquitto::subscribeHandlerRunInThread,&mq);
    th1.detach();

    while(mq._subscribed == false){
    }

    for(int i = 1; i < ilosc+1 ; ++ i)
    {
        mq.publish(topicPublish, "test message: " + std::to_string(i),0);
        std::cout << "wysłano wiadomosc numer: " << i << std::endl;
    }
    puts("start sleep");
    std::this_thread::sleep_for(std::chrono::seconds(7));
    puts("stop sleep");
    int ssize = mq.getReceiveQueueSize();
    if( ssize != ilosc){
        FAIL() <<"error nie odebralem wszystkich wiadomosci " << ssize << std::endl;
    }
    else {
        std::cout << "gra ilosc wiadomosci odebranych " << ssize << std::endl;
    }

    for(int i = 1; i < ssize+1; ++i){
        std::cout << "w forze: " << i << std::endl;
        auto pairMSG = mq.getMessage();
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
