#include <Arduino.h>
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined (_VARIANT_ARDUINO_ZERO_)
 #include <SoftwareSerial.h>
#endif

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
#include "BluefruitConfig.h"

#define FACTORYRESET_ENABLE         1
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define MODE_LED_BEHAVIOUR          "MODE"

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

const int SETUPPINS [] = {2,3};
// DIR, STE            halo,  cfl41, led27, led40, inc, cfl27 
// 191 halo, 223 cfl27, 239 inc, 247 led40, 251 led27, cfl40

const int LIGHTPINS [] = {253, 239, 191, 251, 247, 223};

// 5,6,9,10 pins are available
const int latchPin = 5;
const int clockPin = 6;
const int dataPin = 9;

const int SAMPLESIZE = 2; //Number of pads.
const int MOTORSPEED = 32000; //Faster means less torque. Slower means less time to perform scans.
const int TIMEMARGIN = 500; //+/- value in milliseconds for when the main loop determines a timepoint is reached.
const int ScanWaitTime = 0; //not necessary once phone check is in place. Set it to 0 then.

void error(const __FlashStringHelper*err) {
 Serial.println(err);
 while (1);
}

void setup(void)
{
 setUart();
 setLights();
}

void setLights(){
 Serial.println("Setting pins");
 digitalWrite(SETUPPINS[0], LOW);
 pinMode(SETUPPINS[0], OUTPUT);
 pinMode(SETUPPINS[1], OUTPUT);
 pinMode(latchPin, OUTPUT);
 pinMode(clockPin, OUTPUT);
 pinMode(dataPin, OUTPUT);
 for(int i = 0; i < sizeof(LIGHTPINS) / sizeof(int); i++ ){
   digitalWrite(latchPin, LOW);
   shiftOut(dataPin, clockPin, MSBFIRST, LIGHTPINS[i]);  
   digitalWrite(latchPin, HIGH);
 }
 Serial.println("Waiting for first time point.");
 waitForSignal();
}

void setPin(int pin){
 pinMode(pin, OUTPUT);
 digitalWrite(pin, HIGH);
}

void setUart(){
 delay(500);
 Serial.begin(115200);
 Serial.print(F("Initialising the Bluefruit LE module: "));
 if ( !ble.begin(VERBOSE_MODE) ){
   error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
 }
 Serial.println( F("OK!") );
 if ( FACTORYRESET_ENABLE ){
   Serial.println(F("Performing a factory reset: "));
   if ( ! ble.factoryReset() ){
     error(F("Couldn't factory reset"));
   }
 }
 ble.sendCommandCheckOK(F( "AT+GAPDEVNAME=Stepper" ));
 ble.echo(false);
 ble.info();
 ble.verbose(true);
 while (! ble.isConnected()) {
     delay(500);
 }
 Serial.println(F("******************************"));
 Serial.println( F("Switching to DATA mode.") );
 ble.setMode(BLUEFRUIT_MODE_DATA);
 Serial.println(F("******************************"));
}

void waitForSignal(){
 Serial.println("Waiting for signal");
 bool receivedSignal = false;
 while(receivedSignal == false){
   while ( ble.available() ){
     char c = ble.read();
     Serial.print((char)c);
     if (c == '1'){
       Serial.print("Signal recieved.");
       receivedSignal = true;
     }
   }
 }
}

void timePointCycle(int padStep, int remainStep){
 for (int j = 0; j < sizeof(LIGHTPINS) / sizeof(int); j ++){
   digitalWrite(latchPin, LOW);
   
   for(int k = 0; k < SAMPLESIZE; k++){      
     Serial.print("On Pin ");
     Serial.println(j);
     Serial.println("Cycling");
     waitForSignal();
     delay(ScanWaitTime);
     cycle(padStep);      
   }
   
   Serial.println("Remain Cycle");
   cycle(remainStep);
   shiftOut(dataPin, clockPin, MSBFIRST, LIGHTPINS[j]);  
   digitalWrite(latchPin, HIGH);
 }
}

int cycle(int stepDist){
 for(int n = 0 ; n < stepDist; n++){
   digitalWrite(SETUPPINS[1], LOW);
   digitalWrite(SETUPPINS[1], HIGH);
   delayMicroseconds(MOTORSPEED);
 }
}

void loop(){
  int padStep=1600/SAMPLESIZE; //Motor has 1600 steps.
  int remainStep=1600%padStep;
  unsigned long timeNow=millis();
  timePointCycle(padStep, remainStep);  
  Serial.println("Waiting for next time point.");  
}

