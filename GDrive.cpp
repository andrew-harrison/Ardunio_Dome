#include <Arduino.h>
#include <Ethernet.h>

#include "DOME.h"

void SendToDrive(SensData data){
  char serverName[] = "api.pushingbox.com";
  char devID[] = "vE07EE5D5E30E8D7"; // Unique ID for PushingBox
  unsigned long startTime = millis();
  Serial.println("Connecting to PushingBox...");
  
  EthernetClient pushing_client;
  
  if(pushing_client.connect(serverName, 80)){
    Serial.println("Connected to PushingBox!");

//    pushing_client.print("api.pushingbox.com/pushingbox?devid=vE07EE5D5E30E8D7&tempData=19");
    pushing_client.print("GET /pushingbox?devid=");
    pushing_client.print(devID);
    pushing_client.print("&tempData=");
    pushing_client.print(data.temp[0]);
    pushing_client.println(" HTTP/1.1");
    pushing_client.print("Host: ");
    pushing_client.println(serverName);
    pushing_client.println("User-Agent: Arduino Mega");
    pushing_client.println();
    delay(1);
    //pushing_client.stop();
  }
  
  Serial.print("Response from PushingBox: ");
  while(pushing_client.available()){
        char c = pushing_client.read();
        Serial.print(c);
    if((millis()-startTime)>1000){
        Serial.print("Pushing Box Timeout");
        break;
     }
  }
  
}
