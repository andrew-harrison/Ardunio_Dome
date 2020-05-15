#include <Arduino.h>
#include <Ethernet.h>
#include <SPI.h>

#include "DDNS.h"

#ifndef DOME_H /*Include guards */
#define DOME_H

// Constants
#define V_CONV 0.0048876 //Conversion of 10bit int to 0-5V value

// I/O Pin definitions
#define WHITE 6
#define BLUE 7
#define RED1 8
#define RED2 9

#define MOTOR 22

#define REF_in A0
#define WHITE_in A1
#define BLUE_in A2
#define RED1_in A3
#define RED2_in A4

#define LED 22

#define SOIL A5 // Range 500(wet) to 800 (dry)
#define WATER_LEVEL A6 // 0(dry) to 

//Operating volatges of LEDs where 1023 is 5v
#define W_MAX 735
#define W_MIN 500

#define B_MAX 817
#define B_MIN 500

#define R_MAX 571
#define R_MIN 326






typedef struct
{
  float temp[10];
  float moist[10];
  int LED_Ref_Voltages[4];
} SensData;

// Webpage functions
void Update_Webpage(EthernetServer server);
void Server_Requests(EthernetClient client);
void Update_Sensor_Info(EthernetClient client);
void Change_Light_State(String readString);
void HTML_page(EthernetClient client);


// Main functions
void SendToGDrive(SensData data);

int GetPowerLevel();
void BrightnessController(int*v_now, int*v_ref, byte*brightness);
void SetLights(byte brightness[4]);
void ReadLights(int*);

// bool Soil_Measure(byte trig);

// Debug functions
void GlowLights(byte brightness[4]);

#endif
