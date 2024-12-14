#include "Joystick.h"
#include <EEPROM.h>

#define JOYSTICK_COUNT 1

const int collectivePin = A1; 
const int throttlePin = A0;   

// variables:
//COLLECTIVE
int sensitivity = 12;
int collectiveminaddress = 0;
int collectiveMin = 1023;   
int collectivemaxaddress = 1;    
int collectiveMax = 0;

int lastValue = 0;          
//Throttle
int tminaddress = 2;
int tMin = 1023;   
int tmaxaddress = 3;    
int tMax = 0;
//eeprom save interval
bool flagUpdate = false;
unsigned long saveIntervalEeprom = 300000;          

Joystick_ Joystick(0x03,
                  JOYSTICK_TYPE_MULTI_AXIS, 20, 1,
                  false, false, true, false, 
                  false, false, false, true, 
                  false, false, false);

const unsigned long gcCycleDelta = 1000;
const unsigned long gcAnalogDelta = 25;
const unsigned long gcButtonDelta = 500;
unsigned long gNextTime = 0;
unsigned int gCurrentStep = 0;

// Matrix
boolean blastValue[20];
byte cols[] =  {9,7,8,6};
byte rows[] = {2,5,4,3};
const int rowCount = sizeof(rows)/sizeof(rows[0]);
const int colCount = sizeof(cols)/sizeof(cols[0]);

void setup() {
  // put your setup code here, to run once:
    Joystick.begin();

// Check if there is eeprom data saved.
  if ( EEPROM.length() == 4) {
  //  collectiveMin = EEPROM.read(collectiveminaddress) * 4;
   // collectiveMax = EEPROM.read(collectivemaxaddress) * 4;
  //  tMin = EEPROM.read(tminaddress)* 4;
  //  tMax = EEPROM.read(tmaxaddress)* 4;
  } 
  else
  {
      flagUpdate = true; 
  }

   for(int x=0; x<rowCount; x++) {
    pinMode(rows[x], OUTPUT);
    digitalWrite(rows[x], HIGH);
  }
 
  for (int x=0; x<colCount; x++) {
    pinMode(cols[x], INPUT_PULLUP);
  }

  for (int x=0; x<20; x++) {
    blastValue[x] = false;
  }
    
    
}
void updateEeprom()
{
//  EEPROM.update(collectiveminaddress, collectiveMin / 4);
//  EEPROM.update(collectivemaxaddress, collectiveMax / 4);
//  EEPROM.update(tminaddress, tMin / 4);
//  EEPROM.update(tmaxaddress, tMax / 4);
  
}

void readMatrix()
{
  int pos = 0;
  for (int rowIndex=0; rowIndex < rowCount; rowIndex++) {
   byte rowCol = rows[rowIndex];
   digitalWrite(rowCol, LOW);
   for (int colIndex=0; colIndex < colCount; colIndex++) {
    byte curCol = cols[colIndex];    
    byte _val = digitalRead(curCol);
     if ( _val == LOW) 
      {
      // turn on
       if(blastValue[pos] != true)
       {
        if( pos == 12)
        {
             Joystick.releaseButton(pos);  
        }
        else
        {
          Joystick.pressButton(pos);    
        }
         blastValue[pos] = true;
       }
      }
     else
     {
      //Turn off
        if(blastValue[pos] != false)
        {
          if( pos == 12)
          {
            Joystick.pressButton(pos);    
          }
          else
          {
            Joystick.releaseButton(pos);  
          }
        blastValue[pos] = false;  
        }
     }
    pos++;
    }
    

   digitalWrite(rowCol, HIGH);
   //delay(100);
}
  
}


int analogInput( int pin, int  &sensorMax, int &sensorMin )
{
    int sensorValue = analogRead(pin);
    // record the maximum sensor value
    if (sensorValue > sensorMax) {
      sensorMax = sensorValue;
      flagUpdate = true; 
    }
    if (sensorValue < sensorMin) {
      sensorMin = sensorValue;
      flagUpdate = true; 
    }
    
  // apply the calibration to the sensor reading
  sensorValue = map(sensorValue, sensorMin, sensorMax, 0, 1023);
  // in case the sensor value is outside the range seen during calibration
  sensorValue = constrain(sensorValue, 0, 1023);
  return sensorValue;
}
void loop() {

  int collectiveValue = analogInput( A1, collectiveMax, collectiveMin );
  int diff = abs(collectiveValue - lastValue);
  if( diff > sensitivity)
  {
    Joystick.setZAxis(collectiveValue);  
    lastValue = collectiveValue;
  }
  

  //int throttleValue = analogInput( A0, tMax, tMin );

//  Joystick.setThrottle(throttleValue);

  readMatrix();
  
}
