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
String readString;
short count = 0;
int sensorSmooth = analogRead(0);
bool Button_state = false;
int led = 22;

void HTML_page(EthernetClient client);
void Change_Light_State(String readString);
void Update_Sensor_Info(EthernetClient client);

void setup() {

  Serial.begin(9600);
  Serial.println("Starting-up");
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  pinMode(led, OUTPUT);
  
  Ethernet.begin(mac, ip, subnet, gateway);
  
  
  // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // no point in carrying on, so do nothing forevermore:
    for(;;)
      ;
  }  
    
  Ethernet.begin(mac); //
  server.begin(); // start to listen for clients
  Serial.print("Server is at: ");
  Serial.println(Ethernet.localIP());

 Serial.println("Setting up DDNS");
 EasyDDNS.service("dynu");
 EasyDDNS.client("domeproject.webredirect.org","drew530","ARDUINOupdate");    // Enter ddns Hostname - Username - Client-key
 Serial.println("DDNS Set up");
}

void loop() {
  EthernetClient client = server.available();
  EasyDDNS.update(600000); //Every 10 minutes
  if (client.available())
  {
    char c = client.read();
    while (c != '\n' && client.available()){
    c = client.read();
    readString += c;
    Serial.write(c);
    delay(1);
    }
    
    Change_Light_State(readString);          
    readString=""; //clearing request
  
    HTML_page (client);
    Update_Sensor_Info (client);
    
  }

  client.stop();
  delay(1);

  
   time = millis(); 
   if (time%5000 == 0){
     SensData data;
     data.temp[0] = 1;
     SendToDrive(data);
     
     Serial.println("");
     Serial.print("Current data is: ");
     Serial.println(data.temp[0]);

   }
  
}

void HTML_page(EthernetClient client){

  // send a standard http response header
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  // client.println("Connnection: close");
  client.println();

  client.println("<!DOCTYPE html>"); //web page is made using HTML
  client.println("<html>");
  client.println("<meta http-equiv=\"refresh\" content=\"2\">");
  client.println("<head>");
  client.println("<title>Ethernet Tests</title>");
  client.println("</head>");
  client.println("<hr /> <br />");
  client.println("<center>");
  client.println("<body>");
  client.println("<h1>First Attempts to read/write to Arduino </h1>");
  client.println("<h2>Observing State Of Dial</h2>");
  client.print("<h2>Dial is:  </2>");

  client.println("<a href=\"/?lighton\"\">Turn On Light</a>");
  client.println("<br /> <br />");
  client.println("<a href=\"/?lightoff\"\">Turn Off Light</a><br />");     

  client.println("</body>");
  client.println("</html>");
  
}

void Change_Light_State(String readString){
  if(readString.indexOf("?lighton") >0)//checks for on
  {
    digitalWrite(led, HIGH);    // set pin 8 high
  }
  if(readString.indexOf("?lightoff") >0)//checks for off
    {
      digitalWrite(led, LOW);    // set pin 8 low
    }
}

void Update_Sensor_Info(EthernetClient client){
  int sensorReading = analogRead(0);
  sensorSmooth = (99*sensorSmooth + sensorReading)/100;
  client.println("<br />");
  client.print("analog input ");
  client.print(0);
  client.print(" is ");
  client.print(sensorSmooth);
  client.print("<br />");
}
