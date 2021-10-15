/** I2C_Slave_Transmitter.ino
 * 
 * | Arduino | A4 | ---- SDA
 *           | A5 | ---- SCL
 */



#include "all_devices.h"
#ifdef I_AM_MEGA328

#include <CapacitiveSensor.h>
// https://www.pjrc.com/teensy/td_libs_CapacitiveSensor.html

#include <Wire.h>
// https://www.jianshu.com/p/4b1ddefc9006

#include <Arduino.h>

uint8_t device_addr = 0x02;
unsigned long started_timestamp;

#define CHANNELS 15
#define START_PIN 13


unsigned char flags[4];  // Byte[0,1]:  Is Touched ,  [2,3] Died Sensor.




void requestEvent() {  
  Wire.write(&flags[0], 4);
  Serial.println("replied");

  // Wire.write("hello "); // respond with message of 6 bytes
  // as expected by master
}


CapacitiveSensor* cs[CHANNELS];
long cs_value[CHANNELS]; 

void setup_cs(){
  int pins[] = {2,3,4,5,6,7,8,9,10,11,12,14,15,16,17};
  for (int i=0; i<CHANNELS; i++){
    CapacitiveSensor* new_cs = new CapacitiveSensor(13,pins[i]);
    cs[i] = new_cs;
  }
}

void setup()                    
{
  // cs_4_2.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example
  Serial.begin(115200);
  Wire.begin(device_addr);            // join I2C bus as slave (address provided)
  Wire.onRequest(requestEvent);       // register event
  setup_cs();
}


void loop()                    
{
  uint8_t byte_index;
  uint8_t bit_index;
  flags[0]=0;
  flags[1]=0;
  flags[2]=0;
  flags[3]=0;

  long start = millis();
  for(int i=0; i< CHANNELS; i++){
    bit_index = i % 8;
    // Read sensor value
    cs_value[i] = cs[i]->capacitiveSensor(30);
    // Update flags for application
    if(cs_value[i] <= 0){
      // Hardware got something wrong.
      byte_index = i / 8 + 2;
      flags[byte_index] |= 1 << bit_index;
    }else if (cs_value[i] > 8000){
        // Touched!   SetTouchFlagBit(i);
      byte_index = i / 8;
      flags[byte_index] |= 1 << bit_index;
    }
  }


  // Output capacity sensor values.
  Serial.print(millis() - start);        // check on performance in milliseconds
  Serial.print("\t");                    // tab character for debug windown spacing
  for(int i=0; i<CHANNELS; i++){
    Serial.print(cs_value[i]);                  // print sensor output 1
    Serial.print("\t");
  }
  delay(10);                             // arbitrary delay to limit data to serial port 
}


#endif