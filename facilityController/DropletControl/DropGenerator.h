/*
 * DropGenerator.h - Library for droplet generating
 */

#ifndef DropGenerator_h
#define DropGenerator_h
// #include <digitalWriteFast.h>

#include "Arduino.h"

class DropGenerator
{
  public:
    DropGenerator();
    void oneDrop(int impulseTime);
    void largeDropPreparing(int miniImpulseCount, int miniImpulseTime, int betweenImpulsesTime);
 };

 #endif
