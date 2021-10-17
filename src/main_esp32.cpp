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
    for(int i = 0; i<CELLS; i++){
        TouchCell* cell = &obj_i2c_bus.Cells[i];
        if (cell->HasUpdate()){
            // Touch pin changed
            std::string topic="actp/001/02"; 
            std::string payload ="123456";
            uint8_t* pp = (uint8_t* )(&payload);
            pp[0] = cell->CurrentFlags[0];
            pp[1] = cell->CurrentFlags[1];
            pp[2] = cell->LastFlags[0];
            pp[3] = cell->LastFlags[1];
            pp[4] = cell->CurrentFlags[2];
            pp[5] = cell->CurrentFlags[3];

            if ((cell->CurrentFlags[0] ^ cell->LastFlags[0]) == 0x01){
                topic="actp/foot/yongquanxue";
                if (cell->CurrentFlags[0] == 1)
                    payload = "ON";
                else
                    payload = "OFF";
            }else if ((cell->CurrentFlags[0] ^ cell->LastFlags[0]) == 0x10){
                topic="actp/foot/qitaxue";
                if (cell->CurrentFlags[0] == 0x10)
                    payload = "ON";
                else
                    payload = "OFF";
            }
            mqttClient.publish(topic.c_str(), 2, true, payload.c_str());
        }

    }    
}
#endif
