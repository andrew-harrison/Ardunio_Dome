// Ethernet - Version: Latest
#include <Ethernet.h>
#include <SPI.h>
#include "DOME.h"
#include "DDNS.h"

/*
  Ethernet Settings
*/

byte ip[] = {192, 168, 0, 0};
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x85, 0xD9 };
byte subnet[] = { 255, 255, 255, 0 }; //assigning subnet mask
byte gateway[] = { 192, 168, 1, 254 }; //assigning gateway
EthernetServer server(80);                       //server port


unsigned long time;
short count = 0;
bool Button_state = false;

void setup() {

  Serial.begin(9600);
  Serial.println("Starting-up");
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  pinMode(led, OUTPUT);
  
  //Start the Ethernet connection:
  //Ethernet.begin(mac, ip, subnet, gateway);
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    for(;;) //do nothing forevermore:
      ;
  }  
    
  Ethernet.begin(mac); //
  server.begin(); // start to listen for clients
  Serial.print("Server is at: ");
  Serial.println(Ethernet.localIP());

 Serial.println("Setting up DDNS");
 EasyDDNS.client("domeproject.webredirect.org","drew530","ARDUINOupdate");    // Enter ddns Hostname - Username - Client-key
 Serial.println("DDNS Set up");
}

void loop() {
  // Updates the Routers IP public IP address every 10 minutes
  EasyDDNS.update(600000);

  Update_Webpage(server);
  
  time = millis(); 
  
  // Record data to GSheet
  if (time%5000 == 0){
    SensData data;
    data.temp[0] = 1;
    SendToDrive(data);
    
    Serial.println("");
    Serial.print("Current data is: ");
    Serial.println(data.temp[0]);

   }
  
}
