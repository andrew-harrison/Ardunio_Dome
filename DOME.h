#include <Arduino.h>

//Always use include guards
#ifndef DOME_H
#define DOME_H

#define led 22

typedef struct
{
  float temp[10];
  float moist[10];
} SensData;

void SendToDrive(SensData data);

// Webpage functions
void Update_Webpage(EthernetServer server);
void Server_Requests(EthernetClient client);
void Update_Sensor_Info(EthernetClient client);
void Change_Light_State(String readString);
void HTML_page(EthernetClient client);


#endif
