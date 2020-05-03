#include <Arduino.h>

//Always use include guards
#ifndef DOME_H
#define DOME_H


typedef struct
{
  float temp[10];
  float moist[10];
} SensData;

void SendToDrive(SensData data);

#endif
