#include "DOME.h"

byte brightness[4] = {0}; //Brightness of WHITE BLUE RED1 RED2 LEDs as PWM value


bool motor_trig = false;
byte soil_trigger = 70; // % Soil moisture level that triggers pump

unsigned long last_trigger;

bool network = false; // Debugging


//  Ethernet Settings
byte ip[] = {192, 168, 0, 0};
byte mac[] = { 0x90, 0xA2, 0xDA, 0x0D, 0x85, 0xD9 };
byte subnet[] = { 255, 255, 255, 0 }; //assigning subnet mask
byte gateway[] = { 192, 168, 1, 254 }; //assigning gateway
EthernetServer server(80);                       //server port


void setup() {

  Serial.begin(9600);
  Serial.println("Starting-up");
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  
  pinMode(LED, OUTPUT);
  
  pinMode(WHITE, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(RED1, OUTPUT);
  pinMode(RED2, OUTPUT);

  pinMode(REF_in, INPUT);
  pinMode(WHITE_in, INPUT);
  pinMode(BLUE_in, INPUT);
  pinMode(RED1_in, INPUT);
  pinMode(RED2_in, INPUT);

  pinMode(MOTOR, OUTPUT);

  pinMode(SOIL, INPUT);
  pinMode(WATER_LEVEL, INPUT);
 
 if(network){
  //Ethernet.begin(mac, ip, subnet, gateway);
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    network = false;
    // for(;;); //do nothing forevermore:
  }  

  //Start the Ethernet connection:   
  Ethernet.begin(mac); //
  server.begin(); // start to listen for clients
  Serial.print("Server is at: ");
  Serial.println(Ethernet.localIP());

  Serial.println("Setting up DDNS");
  EasyDDNS.client("domeproject.webredirect.org","drew530","ARDUINOupdate");    // Enter ddns Hostname - Username - Client-key
  Serial.println("DDNS Set up");
  }
}

void loop() {
  // Updates the Routers IP public IP address every 10 minutes
  if(network){
  EasyDDNS.update(600000);
  Update_Webpage(server);
  }

  unsigned long time = millis(); 

  bool water = false;

  int light_vals[5]; // Supply voltage for lights as well as voltage from across W,B,R1 & R2 LEDs
  int v_ref[4] = {0};
  int power_level = 0; // 0 - 0x3FF LED voltage

  if (GetPowerLevel() != power_level){
    power_level = GetPowerLevel();
    float power_scale = (float)power_level/0x3FF;
    //Voltage reference for WHITE BLUE RED1 RED2 LEDs, where 1023 is 5v
    v_ref[0] = (int)(W_MIN+power_scale*(W_MAX-W_MIN));
    v_ref[1] = (int)(B_MIN+power_scale*(B_MAX-B_MIN));
    v_ref[2] = (int)(R_MIN+power_scale*(R_MAX-R_MIN));
    v_ref[3] = (int)(R_MIN+power_scale*(R_MAX-R_MIN));
    
    for(int i = 0; i<4; i++){
      Serial.print(v_ref[i]);
      Serial.print(" \t");
      Serial.print(brightness[i]);
      Serial.print(" \t");
    }
    Serial.println();
  };
  
  ReadLights(light_vals); // Pointer to first array element sent
  BrightnessController(light_vals, v_ref, brightness);
  SetLights(brightness);
  //GlowLights(brightness);

  //water = Soil_Measure(soil_trigger);
  if(water == true)
  {
    water = !water;
    digitalWrite(MOTOR, HIGH);
    Serial.println("Motor switch");
  };

    // Record data to GSheet every 5 sec
  if (time-last_trigger> 5000){
    last_trigger = time;
    SensData data;
    data.temp[0] = 1;

    for(int i=0; i<4; i++){
      data.LED_Ref_Voltages[i] = v_ref[i];
      Serial.print(data.LED_Ref_Voltages[i]);
      Serial.print("\t");
    };

    Serial.println();
    delay(500);
    
    if(network){
      SendToGDrive(data);
    }
    
    Serial.print("Above data sent to Drive");
   }


}