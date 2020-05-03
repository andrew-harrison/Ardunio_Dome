/*
EasyDDNS Library for ESP8266
See the README file for more details.
Written in 2017 by Ayush Sharma.
This work is licensed under a Creative Commons Attribution-ShareAlike 4.0 International License:
http://creativecommons.org/licenses/by-sa/4.0/
Version 1.5.0
Changelog:
Version 1.0.0 - Made EasyDDNS Library for No-ip and DuckDNS
Version 1.5.0 - Optimized Library and Added Dyndns & Dynu

Edits for ethernet from easy2PC forum post
https://forum.arduino.cc/index.php?topic=462217.0
*/

#include "DDNS.h"
#include <Ethernet.h>
#include <SPI.h>
#include <Arduino.h>

void EasyDDNSClass::service(String ddns_service){
  ddns_choice = ddns_service;
}

void EasyDDNSClass::client(String ddns_domain, String ddns_username, String ddns_password){
  ddns_d = ddns_domain;
  ddns_u = ddns_username;
  ddns_p = ddns_password;
}

void EasyDDNSClass::update(unsigned long ddns_update_interval){

  interval = ddns_update_interval;

  unsigned long currentMillis = millis(); // Calculate Elapsed Time & Trigger
  if(currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;
    
// ######## GET PUBLIC IP ######## //        
  EthernetClient client;
  char server[] = "ipv4bot.whatismyipaddress.com";
  char dynu[] = "api.dynu.com";
  bool rec = false;
  int ip_idx = 0;
  int ip_address[] = {0,0,0,0};
  int ip_idx_str = 0; 
  String ip_string = "000.000.000.000";
  bool DEBUG = false;

  if (client.connect(server,80)) 
  {
    Serial.print("Connected to ");
    Serial.println(server);
    client.println("GET / HTTP/1.1");
    client.println("Host: ipv4bot.whatismyipaddress.com");
    client.println("Connection: close");
    client.println();
  } 
  else {
  // if you didn't get a connection to the server:
  Serial.println("DDNS connection failed");
  }

// ######## Read IP from client ######## //        
  while ( ip_address[3] != 0 ){
    
    if(millis()-currentMillis > 3000){
      Serial.println("DDNS timeout");
      break;
    }
    
    int len = client.available();
    if (len > 0) {
      byte buffer[80];
      if (len > 80) len = 80;
      client.read(buffer, len);
      
      if (DEBUG) {
        Serial.print("|");
        Serial.write(buffer, len); // show in the serial monitor (slows some boards)
        Serial.print("|");
      }
      
      for (int i=0; i<len; i++)
      {
        if (rec)
          {
            ip_string[ip_idx_str] = buffer[i];
            ip_idx_str++;
            if (buffer[i]==0x2E){ ip_idx++;}       //Checking for '.' separator
            else
            {
              ip_address[ip_idx] = ip_address[ip_idx]*10 + buffer[i]-'0'; 
            }
          }
        if (buffer[i-3]==0xD && buffer[i-2]==0xA && buffer[i-1]==0xD && buffer[i]==0xA){rec = true;}      
      }
    }
  }
  ip_string[ip_idx_str] = 0; //Terminates string after IP address

  if(DEBUG){
    Serial.print("My IP address is: ");
    for(int i=0; i<4; i++){
      Serial.print(ip_address[i]);
      Serial.print(".");
    }
    Serial.println("");
    Serial.print("My IP String is: ");
    Serial.println(ip_string);
  }
    
// ######## GENERATE UPDATE URL ######## //
      //update_url = "/nic/update?hostname="+ddns_d+"&myip="+ip_string+"&username="+ddns_u+"&password="+ddns_p+"";}
      update_url = "/nic/update?hostname="+ddns_d+"&myip="+ip_string+"&password="+ddns_p;
// ######## CHECK & UPDATE ######### //       
      if(old_ip!=ip_string){
        
        EthernetClient DNS_client;
        if(DNS_client.connect(dynu,8245))
        {
          DNS_client.print("GET /nic/update?hostname=domeproject.webredirect.org&password=ARDUINOupdate HTTP/1.1");
          Serial.println("Request to change sent");

//          Uncecessful attempts to recieve a message back from Dynu server
//          Serial.print("Recieved message: ");
//           int len = client.available();
//           Serial.print("len = ");
//           Serial.println(len);
//           while (DNS_client.available())
//           {
//            char c = DNS_client.read();
//            Serial.print(c);
//            delay(5);
//           }
          old_ip = ip_string;
          DNS_client.stop();
        }       
        else{
          Serial.println("Failed to connect to dynu");
        }
      }

}
else{}
}
EasyDDNSClass EasyDDNS;
