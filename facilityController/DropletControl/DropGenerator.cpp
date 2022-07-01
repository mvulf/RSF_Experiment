#include "Arduino.h"
#include "DropGenerator.h"
// #include <digitalWriteFast.h>

#define pinOrange 6
#define pinBrown 7
#define pinBlue 9
#define pinGreen 10


DropGenerator::DropGenerator()
{
  pinMode(pinOrange, OUTPUT);
  pinMode(pinBrown, OUTPUT);
  pinMode(pinBlue, OUTPUT);
  pinMode(pinGreen, OUTPUT);
}

void DropGenerator::oneDrop(int impulseTime)
{
  // init - LOW
  digitalWrite(pinBlue, LOW);
  digitalWrite(pinGreen, LOW);
  digitalWrite(pinBrown, LOW);
  digitalWrite(pinOrange, LOW);
  
  // OPEN NOZZLE - IMPULSE UP: piezo element's power on
  // black wire - power on
  digitalWrite(pinBlue, HIGH);
  // red wire - to ground
  digitalWrite(pinGreen, HIGH);
  // Hold piezo element's power on during impulse time (black wire ON!)
  delay(impulseTime); 
  
  // CLOSE NOZZLE - IMPULSE DOWN: piezo element's power off
  // black wire - power off
  digitalWrite(pinBlue, LOW);
  // black wire - to ground (piezo element's discharge)
  digitalWrite(pinBrown, HIGH);
  // Hold on piezo element's connection to ground
  delay(5);
  // Unplug black and red wires from ground
  digitalWrite(pinGreen, LOW);
  digitalWrite(pinBrown, LOW);
  // Hold on piezo element without connection
  delay(10);//ms
  
  // REVERSE IMPULSE - UP: change piezo element's connection for total discharging
  // red wire - power on
  digitalWrite(pinOrange, HIGH);
  // black wire - to ground
  digitalWrite(pinBrown, HIGH);
  // Hold piezo element's reverse power on (red wire ON!)
  delay(5);

  // REVERSE IMPULSE - DOWN: piezo element's reversed power off
  // red wire - power off
  digitalWrite(pinOrange, LOW);
  // red wire - to ground (piezo element's discharge)
  digitalWrite(pinGreen, HIGH);
  // Hold on piezo element's connection to ground
  delay(5);
  // Unplug black and red wires from ground
  digitalWrite(pinBrown, LOW);
  digitalWrite(pinGreen, LOW);
}

// miniImpulseTime, betweenImpulsesTime, ms; 
void DropGenerator::largeDropPreparing(int miniImpulseCount, int miniImpulseTime, int betweenImpulsesTime)
{
  // init impulse counter
  int r = 0;

  // generate mini impulses
  while(r<miniImpulseCount)
  {
    oneDrop(miniImpulseTime);
    delay(betweenImpulsesTime);
    r++;
  }
}
