/** I2C_Slave_Transmitter.ino
 * 
 * | Arduino | A4 | ---- SDA
 *           | A5 | ---- SCL
 */



#include "all_devices.h"
#ifdef I_AM_ACUPUNCTURE_MEGA328

// #include <CapacitiveSensor.h>
#include "libs/capacitive_sensor/CapacitiveSensor.h"
// https://www.pjrc.com/teensy/td_li bs_CapacitiveSensor.html

#include <Wire.h>
// https://www.jianshu.com/p/4b1ddefc9006

#include <Arduino.h>
#define I2C_REPLY_BYTES 4
#define CHANNELS 16
#define PIN_CAPACITY_SENSOR_COMMON 12


unsigned char flags[4];  // Byte[0,1]:  Is Touched ,  [2,3] Died Sensor.
unsigned long started_timestamp;

long start;
uint8_t byte_index;
uint8_t bit_index;


void requestEvent() {  
	Wire.write(&flags[0], I2C_REPLY_BYTES);
	Serial.print("replied");
	Serial.println(flags[0], BIN);

	// Wire.write("hello "); // respond with message of 6 bytes
	// as expected by master
}


CapacitiveSensor* cs[CHANNELS];
long cs_value[CHANNELS]; 

void setup_capacity_sensor(){
	int pins[] = {2,3,4,5, 6,7,8,9, 10,11,14,15, 16,17,18,19};   // 14:A0, 15:A1, 16:A2, 17:A3, 18:A4, 19:A5

	for (int i=0; i<CHANNELS; i++){
		CapacitiveSensor* new_cs = new CapacitiveSensor(PIN_CAPACITY_SENSOR_COMMON, pins[i]);
		cs[i] = new_cs;
	}
}

void capcity_sensor_loop(){
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

// uint8_t device_addr = 0x04;

void setup()                    
{
	// obj.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example
	Serial.begin(115200);
	Serial.println("  Hi boys and girl, be happy!");
	Wire.begin(MY_I2C_ADDR);            // join I2C bus as slave (address provided)
	Wire.onRequest(requestEvent);       // register event
	setup_capacity_sensor();

	flags[2] = 0x00;  // 0 is OK, 1 is died.
	flags[3] = 0x00; 
	Serial.println("Setup is finished.");
}

void loop(){
	delay(100);
	capcity_sensor_loop();
	Debug_info();

}
#endif