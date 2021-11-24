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
#ifdef APP_ACTUPUCTURE
	#define MY_I2C_ADDR 0x3
	#define I2C_REPLY_BYTES 4
	#define CHANNELS 16
	#define START_PIN 12
#endif
#ifdef APP_AGV_GARMENT
	#define MY_I2C_ADDR 0x3f
	#define I2C_REPLY_BYTES 2
	#include "hc_sr04.h"
	#define PIN_TRIG 11
	#define PIN_ECHO 10
	HcSr04 obstacle_sensor(PIN_TRIG, PIN_ECHO);
	#include <MFRC522.h>

	#define RST_PIN         9           // Configurable, see typical pin layout above
	#define SS_PIN          10          // Configurable, see typical pin layout above

	MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

	MFRC522::MIFARE_Key key;

#endif

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

#ifdef APP_ACTUPUCTURE

CapacitiveSensor* cs[CHANNELS];
long cs_value[CHANNELS]; 

void setup_capacity_sensor(){
	int pins[] = {2,3,4,5, 6,7,8,9, 10,11,14,15, 16,17,18,19};
	for (int i=0; i<CHANNELS; i++){
		CapacitiveSensor* new_cs = new CapacitiveSensor(START_PIN, pins[i]);
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
#endif

#ifdef APP_AGV_GARMENT
	#define I2C_REPLY_BYTES 1

void setup_garment_bot(){
	pinMode(2, INPUT);
	pinMode(3, INPUT);
	pinMode(4, INPUT);
	pinMode(5, INPUT);
	pinMode(6, INPUT);
	pinMode(7, INPUT);
	pinMode(8, INPUT);
	pinMode(9, INPUT);
}

void agv_garment_sensor_loop(bool show_debug_with_delay){
	flags[0] = 0;
	if(digitalRead(2)) flags[0] |= (1 << 0);
	if(digitalRead(3)) flags[0] |= (1 << 1);
	if(digitalRead(4)) flags[0] |= (1 << 2);
	if(digitalRead(5)) flags[0] |= (1 << 3);
	if(digitalRead(6)) flags[0] |= (1 << 4);
	if(digitalRead(7)) flags[0] |= (1 << 5);
	if(digitalRead(8)) flags[0] |= (1 << 6);
	if(digitalRead(9)) flags[0] |= (1 << 7);

	if (show_debug_with_delay){
		Serial.println(flags[0],BIN);
		delay(500);
	}

	flags[1] = obstacle_sensor.CheckDistance(false);
}

/**
 * Helper routine to dump a byte array as hex values to Serial.
 */
void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

void loop_rf522() {
    // Reset the loop if no new card present on the sensor/reader. This saves the entire process when idle.
    if ( ! mfrc522.PICC_IsNewCardPresent())
        return;

    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial())
        return;

    // Show some details of the PICC (that is: the tag/card)
    Serial.print(F("Card UID:"));
    dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
    Serial.println();
    Serial.print(F("PICC type: "));
    MFRC522::PICC_Type piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    Serial.println(mfrc522.PICC_GetTypeName(piccType));

    // Check for compatibility
    if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_1K
        &&  piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
        Serial.println(F("This sample only works with MIFARE Classic cards."));
        return;
    }

    // In this sample we use the second sector,
    // that is: sector #1, covering block #4 up to and including block #7
    byte sector         = 1;
    byte blockAddr      = 4;
    byte dataBlock[]    = {
        0x01, 0x02, 0x03, 0x04, //  1,  2,   3,  4,
        0x05, 0x06, 0x07, 0x08, //  5,  6,   7,  8,
        0x09, 0x0a, 0xff, 0x0b, //  9, 10, 255, 11,
        0x0c, 0x0d, 0x0e, 0x0f  // 12, 13, 14, 15
    };
    byte trailerBlock   = 7;
    MFRC522::StatusCode status;
    byte buffer[18];
    byte size = sizeof(buffer);

    // Authenticate using key A
    Serial.println(F("Authenticating using key A..."));
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    // Show the whole sector as it currently is
    Serial.println(F("Current data in sector:"));
    mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
    Serial.println();

    // Read data from the block
    Serial.print(F("Reading data from block ")); Serial.print(blockAddr);
    Serial.println(F(" ..."));
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    Serial.print(F("Data in block ")); Serial.print(blockAddr); Serial.println(F(":"));
    dump_byte_array(buffer, 16); Serial.println();
    Serial.println();

    // Authenticate using key B
    Serial.println(F("Authenticating again using key B..."));
    status = (MFRC522::StatusCode) mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("PCD_Authenticate() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
        return;
    }

    // Write data to the block
    Serial.print(F("Writing data into block ")); Serial.print(blockAddr);
    Serial.println(F(" ..."));
    dump_byte_array(dataBlock, 16); Serial.println();
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Write(blockAddr, dataBlock, 16);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Write() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    Serial.println();

    // Read data from the block (again, should now be what we have written)
    Serial.print(F("Reading data from block ")); Serial.print(blockAddr);
    Serial.println(F(" ..."));
    status = (MFRC522::StatusCode) mfrc522.MIFARE_Read(blockAddr, buffer, &size);
    if (status != MFRC522::STATUS_OK) {
        Serial.print(F("MIFARE_Read() failed: "));
        Serial.println(mfrc522.GetStatusCodeName(status));
    }
    Serial.print(F("Data in block ")); Serial.print(blockAddr); Serial.println(F(":"));
    dump_byte_array(buffer, 16); Serial.println();

    // Check that data in block is what we have written
    // by counting the number of bytes that are equal
    Serial.println(F("Checking result..."));
    byte count = 0;
    for (byte i = 0; i < 16; i++) {
        // Compare buffer (= what we've read) with dataBlock (= what we've written)
        if (buffer[i] == dataBlock[i])
            count++;
    }
    Serial.print(F("Number of bytes that match = ")); Serial.println(count);
    if (count == 16) {
        Serial.println(F("Success :-)"));
    } else {
        Serial.println(F("Failure, no match :-("));
        Serial.println(F("  perhaps the write didn't work properly..."));
    }
    Serial.println();

    // Dump the sector data
    Serial.println(F("Current data in sector:"));
    mfrc522.PICC_DumpMifareClassicSectorToSerial(&(mfrc522.uid), &key, sector);
    Serial.println();

    // Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
}

#endif
// uint8_t device_addr = 0x04;

void setup()                    
{
	// obj.set_CS_AutocaL_Millis(0xFFFFFFFF);     // turn off autocalibrate on channel 1 - just as an example
	Serial.begin(115200);
	Serial.println("  Hi boys and girl, be happy!");
	Wire.begin(MY_I2C_ADDR);            // join I2C bus as slave (address provided)
	Wire.onRequest(requestEvent);       // register event
	#ifdef APP_ACTUPUCTURE
		setup_capacity_sensor();
	#endif
	#ifdef APP_AGV_GARMENT
		setup_garment_bot();
	#endif
	flags[2] = 0x00;  // 0 is OK, 1 is died.
	flags[3] = 0x00; 
	Serial.println("Setup is finished.");
}

void loop(){
	delay(100);
	#ifdef APP_ACTUPUCTURE
		capcity_sensor_loop();
		Debug_info();
	#endif
	#ifdef APP_AGV_GARMENT
		agv_garment_sensor_loop(false);
	#endif
}
#endif