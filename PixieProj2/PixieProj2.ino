//const string PINNAMES [] = {“DIR”,”STE”,”SLP”,“CFL27”,”CFL41”,”INC”,”HALO”,”LED27”,”LED40”};

const int SETUPPINS [] = {8,9,10};  //Could put all the pins in one array. Do that.
const int LIGHTPINS [] = {2,3,4,5,6,7}; //pins 0 and 1 are not usable
const int TIMEPOINTS [] = {1, 15, 30, 45, 60, 75, 90, 105, 120, 135, 150, 165, 180 }; //In minutes.
const int SCANWAITTIME = 5000;
const int SAMPLESIZE = 6;
const int MOTORSPEED = 16000;
const int TIMEMARGIN = 500;

//Please note that the return value for millis() is an unsigned long, logic errors may occur if a 
//programmer tries to do arithmetic with smaller data types such as ints. Even signed long may 
//encounter errors as its maximum value is half that of its unsigned counterpart.

void setup(){
  Serial.begin(9600); //For debugging.
  digitalWrite(SETUPPINS[0], LOW);
  pinMode(SETUPPINS[0], OUTPUT);
  pinMode(SETUPPINS[1], OUTPUT);
  for( int i = 0; i < sizeof(LIGHTPINS) / sizeof(int); i++ ){
    setPin(LIGHTPINS[i]);
  }
}

void setPin(int pin){
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
}

int cycle(int stepDist){
  for(int n = 0 ; n <= stepDist; n++){
    digitalWrite(SETUPPINS[1], LOW);
    digitalWrite(SETUPPINS[1], HIGH);
    delayMicroseconds(MOTORSPEED);
  }
  delay(SCANWAITTIME);
}

void timePointCycle(int padStep, int remainStep){
  for (int j = 0; j < ((sizeof(LIGHTPINS) / sizeof(int))); j ++){ //minus 1?
    digitalWrite(LIGHTPINS[j], LOW);
    for(int k = 0; k < SAMPLESIZE; k++){
      cycle(padStep);
    }
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
    }
  }
}
