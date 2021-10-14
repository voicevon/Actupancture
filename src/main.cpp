/*

                                           
                    
                    
 Finger ----------- |------------- Gpio 
                    |
                    |
                   200K
                    |
                    |
                   Gnd


1 Out High
2 delay 1ms
3 Pinmode to input
4 When it got Low, check the time

*/

// https://www.electrosoftcloud.com/en/pcint-interrupts-on-arduino/
// https://www.jianshu.com/p/4b1ddefc9006

#include <Arduino.h>
#include <Wire.h>

uint8_t device_addr = 0x02;
unsigned long started_timestamp;

#define CHANNELS 2
#define START_PIN 4

unsigned char touch_flags[3];
unsigned char is_good_flags[3];
unsigned int timers[CHANNELS];
int touch_pins[CHANNELS];
bool during_high_output;


void assign_pins(){
  touch_pins[0] = 6;   //PD6
  touch_pins[1] = 7;   //PD7
}

void Output_High(){
    int high_time = 1000;   //ms
    // cli();  //Will effect delay.
    during_high_output = true;
    // digitalWrite(START_PIN, HIGH);
    digitalWrite(START_PIN, LOW);
    for(int i=0; i<CHANNELS; i++){
      // pinMode(touch_pins[i], OUTPUT);
      // digitalWrite(touch_pins[i],HIGH);
      timers[i] = 0;
    }
    delay(high_time);
    // sei();   

    started_timestamp = micros();
    during_high_output = false;
    // digitalWrite(START_PIN, LOW);
    digitalWrite(START_PIN, HIGH);
    // for(int i=0; i < CHANNELS; i++){
    //   pinMode(touch_pins[i], INPUT);
    // }
}


/** I2C_Slave_Transmitter.ino
 * 
 * | Arduino | A4 | ---- SDA
 *           | A5 | ---- SCL
 */

void requestEvent() {  
  Wire.write(&touch_flags[0], 3);
  Serial.println("replied");

  // Wire.write("hello "); // respond with message of 6 bytes
  // as expected by master
}

void update_touch_flags(){
  Serial.print("     channel, us = ");
  for (int i=0; i < CHANNELS; i++){
    // Serial.print(i);
    // Serial.print(",");
    Serial.print(timers[i]);
    Serial.print("\t");

  }
}

bool is_all_low(){
  for (int i=0; i < CHANNELS; i++){
    if (timers[i] == 0)
      return false;
  }
  return true;
}

void loo2p(){
  // Output_High(100);
  // unsigned long now = micros();
  unsigned long time_from_last_rise = micros() - started_timestamp;
  if (time_from_last_rise > 1000000){
    // Time out for waiting voltage to fall down.
    // Will effect is_good_flags

    Output_High();
    Serial.println("Timeout   ");
  }else if (is_all_low()){
    Serial.print("   ");
    Serial.print(started_timestamp);
    update_touch_flags();
    Serial.println("");
    // delay(1000);
    Output_High();
  }
}


void setup2() {
  Serial.begin(115200);
  assign_pins();
  Wire.begin(device_addr);            // join I2C bus as slave (address provided)
  Wire.onRequest(requestEvent);       // register event

  // PCICR |= B00000111;
  PCICR |= B00000100;

  // PCMSK0 |= B00000100; // We activate the interrupts on pin D10
  // PCMSK1 |= B00001000; // We activate the interrupts on pin A3
  PCMSK2 |= B11000000; // We activate the interrupts on pin ?
  Output_High();
}


ISR (PCINT0_vect){
  // PortB
  // unsigned now = micros();
  // if (timers[5] == 0)
  //   if (!digitalRead(PB0))
  //     timers[5] = now - started_timestamp;

  // if(timers[6] == 0)
  //   if(!digitalRead(PB1))
  //     timers[6] = now - started_timestamp;

}
ISR (PCINT1_vect){
  // PortC
}
ISR (PCINT2_vect){
  // PortD
  if (during_high_output) return;
  unsigned now = micros();
  for(int i=0; i< CHANNELS; i++){
    if (timers[i] == 0)
      if (digitalRead(touch_pins[i]))
        timers[i] = now - started_timestamp;
  }
  // if(timers[1] == 0)
  //   if(!digitalRead(touch_pins[1]))
  //     timers[1] = now - started_timestamp;
}