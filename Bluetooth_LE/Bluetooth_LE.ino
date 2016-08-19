#include <Arduino.h>
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined(ARDUINO_ARCH_SAMD)
  #include <SoftwareSerial.h>
#endif

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"
#include "BluefruitConfig.h"

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);
#define FACTORYRESET_ENABLE         1


//const string PINNAMES [] = {“DIR”,”STE”,“CFL27”,”CFL41”,”INC”,”HALO”,”LED27”,”LED40”};

const int SETUPPINS [] = {2,3};  //Could put all the pins in one array. Do that.
const int LIGHTPINS [] = {5,6,9,10,11,12}; //pins 0 and 1 are not usable
const int TIMEPOINTS [] = {1, 15, 30, 45, 60, 75, 90, 105, 120, 135, 150, 165, 180}; //In minutes.
const int SAMPLESIZE = 6;
const int MOTORSPEED = 16000;
const int TIMEMARGIN = 500;
const int RANDOMONE = 5;
const int RANDOMTWO = 100;

void setup(){
  Serial.begin(115200);
  Serial.println("Setting pins");
  digitalWrite(SETUPPINS[0], LOW);
  pinMode(SETUPPINS[0], OUTPUT);
  pinMode(SETUPPINS[1], OUTPUT);
  for( int i = 0; i < sizeof(LIGHTPINS) / sizeof(int); i++ ){
    setPin(LIGHTPINS[i]);
  }
  setBluetooth();
}

void setPin(int pin){
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
}

void setBluetooth(){
  ble.begin(VERBOSE_MODE);
  ble.factoryReset();
  ble.echo(false);
  ble.info();
  ble.sendCommandCheckOK(F( "AT+GAPDEVNAME=Stepper" ));
  ble.sendCommandCheckOK(F( "AT+BleHIDEn=On" ));
  ble.setMode(BLUEFRUIT_MODE_DATA);
  ble.reset();
  Serial.println("Waiting for first time point.");
}

void waitForSignal(){
  Serial.println("Waiting for signal");
  bool receivedSignal = false;
  char input; //switch this to a more reasonable data type.
  while(receivedSignal == false){
    input = ble.read();
    if(input == '1'){
      Serial.println("Signal Recieved");
      receivedSignal = true;
    }
  } 
}

int cycle(int stepDist){
  for(int n = 0 ; n <= stepDist; n++){
    digitalWrite(SETUPPINS[1], LOW);
    digitalWrite(SETUPPINS[1], HIGH);
    delayMicroseconds(MOTORSPEED);
  }
}

void timePointCycle(int padStep, int remainStep){
  for (int j = 0; j < ((sizeof(LIGHTPINS) / sizeof(int))); j ++){ //minus 1?
    digitalWrite(LIGHTPINS[j], LOW);
    for(int k = 0; k < SAMPLESIZE; k++){
      Serial.print("On Pin ");
      Serial.println(j);
      Serial.println("Cycling");
      waitForSignal();
      cycle(padStep);
     // randomize();
    }
    Serial.println("Remain Cycle");
    cycle(remainStep);
    digitalWrite(LIGHTPINS[j], HIGH);
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
