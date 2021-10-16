//https://www.electronicshub.org/wp-content/uploads/2021/02/ESP32-Pinout-1.jpg

#include "all_devices.h"
#ifdef I_AM_ESP32

#include "i2c_commu.h"
#include "wifi_mqtt_client.h"
#include <string>
#include <HardwareSerial.h>
// #include <Arduino.h>


I2c_commu obj_i2c_bus = I2c_commu();
// Wifi_MqttClient obj_mqtt = Wifi_MqttClient();

void setup() {
  Serial.begin(115200);
  setup_wifi_mqtt();
  obj_i2c_bus.Init();
//   obj_mqtt.Init();

}
// void mqtt_publish(const char* topic, const char* payload){
//   if (mqttClient.connected()){
//     mqttClient.publish(topic, 2, true, payload);
//   }
// }
void loop() {
    obj_i2c_bus.SpinOnce();
    if (obj_i2c_bus.HasUpdate()){
        // Touch pin changed
        std::string topic="actp/001/02"; 
        std::string payload ="123456";
        uint8_t* pp = (uint8_t* )(&payload);
        pp[0] = obj_i2c_bus.Cells[0].CurrentFlags[0];
        pp[1] = 0x80;
        pp[2] = 0x80;
        pp[3] = 0x80;
        pp[4] = 0x80;

        mqttClient.publish(topic.c_str(), 2, true, payload.c_str());

    }    
}
#endif
