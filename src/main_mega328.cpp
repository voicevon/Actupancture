/** I2C_Slave_Transmitter.ino
 * 
 * | Arduino | A4 | ---- SDA
 *           | A5 | ---- SCL
 */



#include "all_devices.h"
#ifdef I_AM_MEGA328

#include <CapacitiveSensor.h>
// https://www.pjrc.com/teensy/td_li bs_CapacitiveSensor.html

#include <Wire.h>
// https://www.jianshu.com/p/4b1ddefc9006

#include <Arduino.h>


uint8_t device_addr = 0x02;

unsigned long started_timestamp;
#define CHANNELS 16
#define START_PIN 12


unsigned char flags[4];  // Byte[0,1]:  Is Touched ,  [2,3] Died Sensor.
long start;
uint8_t byte_index;
uint8_t bit_index;


void requestEvent() {  
  Wire.write(&flags[0], 4);
  Serial.println("replied");

  // Wire.write("hello "); // respond with message of 6 bytes
  // as expected by master
}


CapacitiveSensor* cs[CHANNELS];
long cs_value[CHANNELS]; 

void setup_cs(){
  int pins[] = {2,3,4,5, 6,7,8,9, 10,11,14,15, 16,17,18,19};
  for (int i=0; i<CHANNELS; i++){
    CapacitiveSensor* new_cs = new CapacitiveSensor(START_PIN, pins[i]);
    cs[i] = new_cs;
  }
}

void setup()                    
{
  // obj.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example
  Serial.begin(115200);
  Wire.begin(device_addr);            // join I2C bus as slave (address provided)
  Wire.onRequest(requestEvent);       // register event
  setup_cs();
  flags[2] = 0x00;  // 0 is OK, 1 is died.
  flags[3] = 0x00; 
}

void cs_loop(){

  uint8_t local_flags[2];
  local_flags[0]=0;
  local_flags[1]=0;

  start = millis();
  long csv= 0;
  for(int i=0; i< CHANNELS; i++){
    bit_index = i % 8;
    byte_index = i / 8 + 2;           
    if((flags[byte_index] & (1 << bit_index)) == 0){
      // Hardware is OK, Read sensor value
      csv = cs[i]->capacitiveSensor(30);
      cs_value[i] = csv;
      // Update flags for application
      if(csv == -2){
        // Hardware got something wrong.
        Serial.print("Channel Got error  ");
        Serial.println(i);
        byte_index = i / 8 + 2;
        flags[byte_index] |= 1 << bit_index;

      }else if (csv > 2000){
        // Touched!   SetTouchFlagBit(i);
        byte_index = i / 8;
        local_flags[byte_index] |= 1 << bit_index;
      }
    }
  }
  flags[0] = local_flags[0];
  flags[1] = local_flags[1];
}
void Debug_info(){
  // Output capacity sensor values.
  Serial.print(millis() - start);        // check on performance in milliseconds
  Serial.print("\t");                    // tab character for debug windown spacing
  for(int i=0; i<CHANNELS; i++){
    bit_index = i % 8;
    byte_index = i / 8 + 2;  
    uint8_t flag = flags[byte_index] & (1<< bit_index);
    if( flag == 0 )      
      Serial.print(cs_value[i]);                  // print sensor output 1
    else
      Serial.print('.');

    Serial.print("\t");
  }
  Serial.print(flags[3],BIN);
  Serial.print("  ");
  Serial.print(flags[2],BIN);
  Serial.print("  ");
  Serial.print(flags[1],BIN);
  Serial.print("  ");
  Serial.print(flags[0],BIN);

  Serial.println("");
  delay(100);                             // arbitrary delay to limit data to serial port 
}

void loop()                    
{
  delay(100);
  cs_loop();
  Debug_info();
}


#endif