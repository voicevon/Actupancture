#pragma once
// #include <stdint.h>
#include "all_devices.h"
// #ifdef USING_WIFI_MQTT


// #include <AsyncMqttClient.h>

extern void setup_wifi_mqtt();
extern AsyncMqttClient mqttClient;

// #endif
// class Wifi_MqttClient{
//     public:
//         void Init();
//         void Init_wifi(const char* wifi_ssid, const char* wifi_pwd);
//         // void Init_mqtt(const char* host, uint16_t port);
    
//     private:
//     void ConnectWifi();
//         const char* __wifi_ssid;
//         const char* __wifi_password;
//         const char* __mqtt_host;
//         uint16_t __port;

// };