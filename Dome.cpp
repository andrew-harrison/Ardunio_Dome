#include <Arduino.h>
#include <Ethernet.h>

#include "DOME.h"

//Ethernet Functions
void SendToGDrive(SensData data){
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

void Update_Webpage(EthernetServer server){
  // Update the ethernet client with
  EthernetClient client = server.available();
  
  // Update local webpage with sensor values and read any output requests
  if (client.available())
  { 
    Server_Requests(client);  
    HTML_page(client);
    Update_Sensor_Info(client);
  }

  client.stop();
}

void Server_Requests(EthernetClient client){
  String readString;

  char c = client.read();
  while (c != '\n' && client.available()){
  c = client.read();
  readString += c;
  Serial.write(c);
  delay(1);
  }

  Change_Light_State(readString);          
  readString=""; //clearing request
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
    digitalWrite(LED, HIGH);    // set pin 8 high
  }
  if(readString.indexOf("?lightoff") >0)//checks for off
    {
      digitalWrite(LED, LOW);    // set pin 8 low
    }
}

void Update_Sensor_Info(EthernetClient client){
  int sensorReading = analogRead(0);
  static int sensorSmooth;

  sensorSmooth = (99*sensorSmooth + sensorReading)/100;
  client.println("<br />");
  client.print("analog input ");
  client.print(0);
  client.print(" is ");
  client.print(sensorSmooth);
  client.print("<br />");
}

// Light Functions
void SetLights(byte *brightness){

    analogWrite(WHITE, brightness[0]);
    analogWrite(BLUE, brightness[1]);
    analogWrite(RED1, brightness[2]);
    analogWrite(RED2, brightness[3]);
}

void ReadLights(int *light_volt){
    
    int ref = analogRead(REF_in);
    *light_volt = ref; // Same as light_volt[0]
    light_volt[1] = ref - analogRead(WHITE_in);
    light_volt[2] = ref - analogRead(BLUE_in);
    light_volt[3] = ref - analogRead(RED1_in);
    light_volt[4] = ref - analogRead(RED2_in);

    // Serial.print(light_volt[0]*V_CONV);
    // Serial.print("\t");
    // Serial.print(light_volt[1]*V_CONV);
    // Serial.print("\t");
    // Serial.print(light_volt[2]*V_CONV);
    // Serial.print("\t");
    // Serial.print(light_volt[3]*V_CONV);
    // Serial.print("\t");
    // Serial.println(light_volt[4]*V_CONV);
}

void BrightnessController(int*v_now, int*v_ref, byte*brightness){
  const byte gain = 1;
  static float f_brightness[4];
  for (int i = 0; i < 4; i++)
  {
    signed int err = v_ref[i] - v_now[i+1]; //+1 because index 0 is + voltage rail
    f_brightness[i] = 0.99*f_brightness[i] + 0.01*(err*gain);
    Serial.print(f_brightness[i]);
    Serial.print(" ");

    if ( f_brightness[i]>= 255){
      brightness[i] = 255;
    }
    else if (f_brightness[i]<= 0){
      brightness[i] = 0;
    }    
    else{
      brightness[i] = (byte)f_brightness[i];
    }

    Serial.print(brightness[i]);
    Serial.print(" ");
    Serial.print(err);
    Serial.print("\t");
  }
  Serial.println("\t");
  delay(20);
}

void GlowLights(byte brightness[4])
{
    delay(25);

    for (byte i = 0; i <= 4; i++)
    {
        if(brightness[i]>=255){
            brightness[i] = 0;
        }
        else
        {
            brightness[i]++;
        }      
    }  
}


/*bool Soil_Trigger(byte trig){
    if (analogRead(SOIL)<trig){ // Value between 0 and 0x3FF, saturates at 2A7

        Serial.print("Soil sensor triggered at: ");
        Serial.print(analogRead(SOIL)/(679.0));
        Serial.println(" %");
        
        return true;
    }
    else
    {
        return false;
    };
     
}
*/

