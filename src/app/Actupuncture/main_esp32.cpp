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
std::string topic = "actu/foot/yongquanxue"; 
std::string payload ="123456";

void loop() {
    obj_i2c_bus.SpinOnce();
    for(int i = 0; i<CELLS; i++){
        TouchCell* cell = &obj_i2c_bus.Cells[i];
        if (cell->HasUpdate()){
            // Touch pin changed
            topic = "actp/";
            topic.append(BODY_ID);
            topic.append("/");
            for (int j=0; j<15; j++){
                if (cell->IsBitUpdated(j)){
                    topic.append(cell->GetName(j));
                    payload = cell->GetMqttPayload(j);
                    mqttClient.publish(topic.c_str(), 2, true, payload.c_str());
                }
            }
            mqttClient.publish(topic.c_str(), 2, true, payload.c_str());
        }
    }  
}
#endif