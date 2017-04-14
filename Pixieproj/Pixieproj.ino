/*This is a program that is designed to carry out an experiment for Pixie Scientific involving 6 or 8
  Pixie Pads to verify their experation date. In the Pixie developer app, this code corresponds to experiment
  type 39. 
  
  The code is written to rotate a plate with pads on it at the following predefined timepoints: 1min, 
  15min, 30min, 1hr, 2hr, 3hr, and 4hr. At each timepoint, the plate will rotate 6 times, once for each lighting
  CFL 2700, CFL 4100, INCANDESCENT, HALOGEN, LED 2700, and LED 4000. 
  
  The sequence activates when the RESET button on the Arduino Uno is pressed. Time begins from that point. 
  
  If possible, please unplug the device when not in use. If left to run overnight, please unplug on entry to 
  the office. This is to avoid overheating and variable overflow.
  
  The motor is set up to make 1600 steps per full rotation. For n items, there must be n-1 rotations to avoid scanning
  the first item twice.
  
  If the experiment requires 6 pads, set the variables: 
  SampleSize to 6
  RotationSize to 266
  Setting it this way will produce 5 (SampleSize-1) rotations of 266 steps that amount to 1330 steps. The remaining steps are accounted
  for in the code to return the plate to its original position.
  
  If the experiment requires 8 pads, set:
  SampleSize to 8
  RotationSize to 200
  This produces 7 (SampleSize-1) rotations totalling 1400 steps to scan all 8 pads. The plate is then returned to original position. */

  
#define CFL27 2                       //CFL 2700 pin
#define CFL41 3                       //CFL 4100 pin
#define INC 4                         //Incandescent pin
#define HALO 5                        //Halogen pin
#define LED27 6                       //LED 2700 pin
#define LED40 7                       //LED 4000 pin
#define DIRpin 8                      //controls direction of rotation
#define STEPpin 9                     //controls stepping
#define SLP 10                        //sleep pin
#define ScanWaitTime 5000             //how long the diaper will stay still for scanning in milliseconds
#define SampleSize 6                  //how many samples used in experiment
#define RotationSize 266              //stepper motor has 1600 steps. RotationSize decides how many steps the motor will turn to scan each diaper.
                                      //if 6, 1600/6=266 remainder 4
                                      //if 8, 1600/8=200
#define MotorSpeed 16000              //speed of motor in terms microseconds of delay between small steps. the lower this number, the greater the speed. optimally 8000-16000


void setup() 
{
Serial.begin(9600);                           //open serial monitor, useful for debugging
digitalWrite(DIRpin, LOW);                    //set direction counter clockwise (arbitrary)
pinMode(STEPpin, OUTPUT);                     
pinMode(DIRpin, OUTPUT);                      //declare both pins activating motor as OUTPUTs
pinMode(CFL27, OUTPUT);
pinMode(CFL41, OUTPUT);
pinMode(INC, OUTPUT);
pinMode(HALO, OUTPUT);
pinMode(LED27, OUTPUT);
pinMode(LED40, OUTPUT);                       //declare all 6 pins activating lightbulbs as OUTPUTS, allowing Arduino to send signal out to turn them on



digitalWrite(CFL27, HIGH);
digitalWrite(CFL41, HIGH);
digitalWrite(INC, HIGH);
digitalWrite(HALO, HIGH);
digitalWrite(LED27, HIGH);
digitalWrite(LED40, HIGH);                   //make sure everything is off initially. HIGH sends 5V to pin which keeps the relay controlling the power plug OFF. Relay activates on LOW.
}                                              


void loop() {
  unsigned long timeNow=millis();                              //define a variable equal to the time since program began running in milliseconds
  Serial.println(timeNow);                                     //print time since program began to Serial Monitor. useful for debugging.
  
/*All time intervals in the void loop if statements are expressed as small intervals because the arduino reads
  through void loop very fast and we want to make sure it catches the desired time within
  half a second of error at most.*/
  
 
if(59500<timeNow && timeNow<60500)                             //after 1 minute. 1000ms*60s = 60,000ms +/- half a second
  {
  RotationLightSequence();                                     //rotates 6 times under different lighting conditions. Function is defined below.
  }
  
else if(timeNow>890500 && timeNow<900500)                      //after 15 minutes. 1000ms*60s*15min = 900,000ms +/- half a second
  {
  RotationLightSequence();
  }
  
else if(timeNow>1790500 && timeNow<1800500)                    //after 30 minutes. 1000ms*60s*30min = 1,800,000ms +/- half a second
  {
  RotationLightSequence();     
  }
  
else if(timeNow>3590500 && timeNow<3600500)                    //after 1 hour. 1000ms*60s*60min = 3,600,000ms +/- half a second
  {
  RotationLightSequence();   
  }
  
else if(timeNow>7190500 && timeNow<7200500)                    //after 2 hours. 1000ms*60s*60min*2hr = 7,200,000ms +/- half a second
  {
  RotationLightSequence(); 
  }
  
else if(timeNow>10790500 && timeNow<10800500)                  //after 3 hours. 1000ms*60s*60min*3hr = 10,800,000ms +/- half a second
  {
  RotationLightSequence();    
  }
  
else if(timeNow>14390500 && timeNow<14400500)                  //after 4 hours. 1000ms*60s*60min*4hr = 14,400,000ms +/- half a second
  {
  RotationLightSequence();   
  }

}


  
/*--------------------------FUNCTION DEFINITIONS-----------------------*/

/*--------------------------ROTATE FUNCTION-----------------------*/

/*Rotate function rotates the shaft of the motor SampleSize times 
  with RotationSize steps per rotation and then makes sure the shaft 
  returns to original starting position by checking if it has made
  all 1600 steps necessary for a full turn. This makes sure the pads
  stay oriented correctly with respect to the phone camera and prevents
  cascading error over several rotations. Note that at this point, a certain
  light is on, depending on where you are in the RotationLightSequence,
  defined below.*/

void Rotate()
{
int RotaryCounter=0;                               //initialize local counter that monitors progression of rotation
  while(RotaryCounter<1329)                        //runs while RotaryCounter is less than 266*5=1330 or 200*7=1400. 1400 works since the 6th rotation registers as 1200 steps, and will run the 7th rotation
  {
    for(int j=1; j<=SampleSize-1; j++)             //rotate 6-1=5 or 8-1=7 times, depending on how SampleSize variable is defined. n-1 rotations because n items make n-1 intervals.
    {
      for(int i=1; i<=RotationSize; i++)           //depending on def of RotationSize variable: rotate almost 1/6 of a full rotation since 1600/6=266 remainder 4 OR rotate 1/8 of a full rotation
      {
        digitalWrite(STEPpin, LOW);                
        digitalWrite(STEPpin, HIGH);               //changing this pin LOW to HIGH is what causes a step to occur
        delayMicroseconds(MotorSpeed);             //this MICROsecond delay is the time between individual 1/1600th steps. This is effectively the motor speed. This number must be kept relatively...
      }                                            //...(from previous line) high in order to make sure the motor produces enough power to rotate the plate+pads.
      
    delay(ScanWaitTime);                           //wait this long in milliseconds so iPhone can scan the pad before repeating the loop
    RotaryCounter=RotaryCounter+RotationSize;      //increment RotaryCounter for counting rotation progression for While loop
    }

digitalWrite(CFL27, HIGH);
digitalWrite(CFL41, HIGH);
digitalWrite(INC, HIGH);
digitalWrite(HALO, HIGH);
digitalWrite(LED27, HIGH);
digitalWrite(LED40, HIGH);                         //make sure  all lights are off after rotation is done. 
Serial.println("Lights off");
delay(1000);                            
 }
 
  if(RotaryCounter<1600)                           //makes sure one rotation has not occured yet
  {
    int difference=1600-RotaryCounter;             //introduce remainder until 1 rotation complete
      for(int i=1; i<=difference; i++)             //step the remaining steps to return to original orientation. Since lights are off at this point, returning to the first pad that was scanned...
      {                                            //...(from prev line) will not scan the pad a second time, until the next lighting condition is turned on.
        digitalWrite(STEPpin, LOW);
        digitalWrite(STEPpin, HIGH);
        delayMicroseconds(MotorSpeed);
      }
  }
  Serial.println("rotation");                      //report that a full rotation has been made to the Serial Monitor. For debugging.
}

/*--------------------------ROTATION LIGHT SEQUENCE-----------------------*/

/*This function uses the Rotate() function (defined above) and implements 
 the entire data collection process for a single timepoint. It changes the 
 lighting conditions in sequence and rotates the plate according to Rotate() 
 so that the phone can collect data at each different lighting condition. 
 Note that turning on a light allows the phone, which is always on, to actually
 collect data via camera. When lights are off, it is obviously too dark to record
 data. Thus light control is a method of controlling when the phone collects data.
 Though lights are turned on within this function, they are turned off within the 
 Rotate() function. The program also makes sure all of the lights are off at the end of the last 
 lighting condition.*/
 
void RotationLightSequence()
{
  digitalWrite(CFL27, LOW);                 //turn on CFL 2700
  Serial.println("CFL 2700 on");            //print which light is on to Serial Monitor. Useful for debugging.
  delay(ScanWaitTime);                      //wait for the 1st scan to occur
    Rotate();                               //rotate and collect data. This function is defined above. After this fcn is executed, all lights are off.
 
  
delay(1000);                                //pause for effect

  digitalWrite(CFL41, LOW);                 //switch CFL 4100 on for 2nd lighting condition
  Serial.println("CFL 4100 on");
  delay(ScanWaitTime);
    Rotate();          
    

delay(1000);                                                  

  digitalWrite(INC, LOW);                   //switch INCANDESCENT on for 3rd lighting condition
  Serial.println("INCANDESCENT on");
  delay(ScanWaitTime);
    Rotate();          
   

delay(1000);                                

  digitalWrite(HALO, LOW);                  //switch HALOGEN on for 4th lighting condition
  Serial.println("HALOGEN on");
  delay(ScanWaitTime);
    Rotate();          
   

delay(1000);                                

  digitalWrite(LED27, LOW);                 //switch LED 2700 on for 5th lighting condition
  Serial.println("LED 2700 on");
  delay(ScanWaitTime);
    Rotate();          
    

delay(1000);                                

  digitalWrite(LED40, LOW);                 //switch LED 4000 on for 6th lighting condition
  Serial.println("LED 4000 on");
  delay(ScanWaitTime);
    Rotate();          


delay(1000);

digitalWrite(CFL27, HIGH);
digitalWrite(CFL41, HIGH);
digitalWrite(INC, HIGH);
digitalWrite(HALO, HIGH);
digitalWrite(LED27, HIGH);
digitalWrite(LED40, HIGH);                   //make sure everything is off
}

/*-------------------------------------------------------------------*/

