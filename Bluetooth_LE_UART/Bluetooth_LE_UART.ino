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

const int SETUPPINS [] = {2,3};     //DIR, STE
const int LIGHTPINS [] = {5,6,9,10,11,12};  //CFL27, CFL41, INC, HALO, LED27, LED40
const int TIMEPOINTS [] = {1, 15, 30, 45, 60, 75, 90, 105, 120, 135, 150, 165, 180};  //In minutes.
const int SAMPLESIZE = 8; //Number of pads.
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
  for( int i = 0; i < sizeof(LIGHTPINS) / sizeof(int); i++ ){
    setPin(LIGHTPINS[i]);
  }
  Serial.println("Waiting for first time point.");
 // waitForSignal();
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
      if (c == 'g'){
        Serial.print("Signal recieved.");
        receivedSignal = true;
      }
    }
  }
}

void timePointCycle(int padStep, int remainStep){
  for (int j = 0; j < ((sizeof(LIGHTPINS) / sizeof(int))); j ++){
    digitalWrite(LIGHTPINS[j], LOW);
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
    digitalWrite(LIGHTPINS[j], HIGH);
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
  for(int m = 0; m < ((sizeof(TIMEPOINTS) / sizeof(int))); m++){
    if(timeNow - TIMEMARGIN <  (TIMEPOINTS[m]*60000) && timeNow + TIMEMARGIN > (TIMEPOINTS[m]*60000)){
      timePointCycle(padStep, remainStep);  
      Serial.println("Waiting for next time point.");  
    }
  }
}
/*
void loop(void){  
  char n, inputs[BUFSIZE+1];

  if (Serial.available())
  {
    n = Serial.readBytes(inputs, BUFSIZE);
    inputs[n] = 0;
    // Send characters to Bluefruit
    Serial.print("Sending: ");
    Serial.println(inputs);

    // Send input data to host via Bluefruit
    ble.print(inputs);
  }

  while ( ble.available() )
  {
    int c = ble.read();

    Serial.print((char)c);

    // Hex output too, helps w/debugging!
    Serial.print(" [0x");
    if (c <= 0xF) Serial.print(F("0"));
    Serial.print(c, HEX);
    Serial.print("] ");
  }
}*/
