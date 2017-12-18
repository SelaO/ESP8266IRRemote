char auth[] = ""; // blynk auth 
char ssid[] = ""; // wifi's name 
char pass[] = "";
/*************************************************************
  Download latest Blynk library here:
    https://github.com/blynkkk/blynk-library/releases/latest

  Blynk is a platform with iOS and Android apps to control
  Arduino, Raspberry Pi and the likes over the Internet.
  You can easily build graphic interfaces for all your
  projects by simply dragging and dropping widgets.

    Downloads, docs, tutorials: http://www.blynk.cc
    Sketch generator:           http://examples.blynk.cc
    Blynk community:            http://community.blynk.cc
    Follow us:                  http://www.fb.com/blynkapp
                                http://twitter.com/blynk_app

  Blynk library is licensed under MIT license
 *************************************************************

/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
#define BLYNK_NO_BUILTIN   // Disable built-in analog & digital pin operations
#define BLYNK_NO_FLOAT     // Disable float operations

#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266_SSL.h>
extern "C" {
  #include "user_interface.h"
}

IRsend irsend(D3);  
BlynkTimer timer;

int khz = 38;
uint16_t ACOnColdCode[] = { 
8936,4440,704,1576,716,480,656,536,660,1624,656,1624,656,1628,652,540,660,536,656,1624,660,1624,656,540,652,540,660,536,656,536,656,540,652,540,652,544,660,536,656,536,656,540,652,1628,652,1632,660,1620,660,536,656,536,656,540,652,544,660,532,656,1628,652,540,652,1632,652,540,660,536,656,1624,656,540,652,19836,656,536,656,540,652,540,652,544,660,532,688,508,684,508,684,512,680,516,688,504,688,508,684,508,684,512,680,1600,660,536,688,504,688,512,680,512,680,512,688,508,684,508,684,512,680,520,684,504,688,504,688,508,684,512,680,512,692,504,688,504,688,508,684,1596,684
};
uint16_t ACOffCode[] = { 
8964,4412,660,1624,656,536,656,540,656,540,648,1632,660,1624,660,532,660,536,688,1592,688,1596,652,540,684,512,680,516,688,504,688,508,684,508,684,512,680,512,684,512,688,508,688,504,688,1596,684,1596,684,512,680,512,692,504,688,504,688,508,684,1596,688,508,684,1600,680,512,680,516,688,1592,688,508,684,19800,688,508,684,512,680,512,680,516,688,504,688,508,684,508,684,512,680,512,680,516,676,516,688,508,684,512,680,1600,684,512,680,512,688,508,684,508,688,508,684,508,684,512,680,516,688,504,688,508,684,508,684,512,680,516,688,504,688,508,684,508,684,512,680,512,680
};
uint16_t ACOnHotCode[] = { 

};
uint16_t FanToggleCode[] = { 
1324,356,1304,376,440,1248,1272,408,1272,416,432,1260,440,1256,444,1252,492,1208,440,1256,440,1252,1280,6968,1272,404,1304,376,476,1212,1304,376,1304,384,468,1228,440,1256,444,1252,436,1260,440,1256,444,1252,1308,6920,1308,372,1300,380,468,1220,1300,380,1300,388,472,1220,468,1228,472,1224,476,1224,464,1232,468,1228,1300,6944,1308,368,1300,380,472,1216,1300,380,1300,388,472,1224,464,1232,468,1228,472,1224,468,1228,472,1224,1304,6952,1300,376,1300,380,472,1216,1300,380,1300,388,464,1228,472,1224,476,1220,468,1232,468,1228,472,1220,1308
};
uint16_t LedStripToggle[] = { 

};

// ACOnCold
BLYNK_WRITE(V5)
{
  int pinValue = param.asInt(); 
  if(pinValue == 1){
    //ACOnCold();
    timer.setTimer(500, ACOnCold, 3);
  }
}

// ACOnCold
BLYNK_WRITE(V1)
{
  int pinValue = param.asInt(); 
  if(pinValue == 1){
    //ACOnCold();
    timer.setTimer(500, ACOnCold, 3);
  }
}

void ACOnCold(){
  Serial.println("ACOnCold");
  irsend.sendRaw(ACOnColdCode, sizeof(ACOnColdCode) / sizeof(ACOnColdCode[0]), khz); 
  // indicate on control led that a transmission is ongoing 
  turnControlLedOn1Sec();
}

// ACOff
BLYNK_WRITE(V2)
{
  int pinValue = param.asInt(); 
  if(pinValue == 1){
    ACOff();
    //timer.setTimer(100, ACOff, 3);  // TODO test this if too slow 
  }
}

void ACOff(){
  Serial.println("ACOff");
  irsend.sendRaw(ACOffCode, sizeof(ACOffCode) / sizeof(ACOffCode[0]), khz); 
  // indicate on control led that a transmission is ongoing 
  turnControlLedOn1Sec();
}

// ACOnHot
BLYNK_WRITE(V3)
{
  int pinValue = param.asInt(); 
  if(pinValue == 1){
    timer.setTimer(100, ACOnHot, 3);  // TODO test this if too slow 
  }
}

void ACOnHot(){
  Serial.println("ACOnHot");
  irsend.sendRaw(ACOnHotCode, sizeof(ACOnHotCode) / sizeof(ACOnHotCode[0]), khz); 
  // indicate on control led that a transmission is ongoing 
  turnControlLedOn1Sec();
}

// fanToggle
BLYNK_WRITE(V4)
{
  int pinValue = param.asInt(); 
  if(pinValue == 1){
    fanToggle();
    //timer.setTimer(100, fanToggle, 3);  // TODO test this if too slow 
  }
}

void fanToggle(){
  Serial.println("fanToggle");
  irsend.sendRaw(FanToggleCode, sizeof(FanToggleCode) / sizeof(FanToggleCode[0]), khz); 
  // indicate on control led that a transmission is ongoing 
  turnControlLedOn1Sec();
}

// control led
BLYNK_WRITE(V0)
{
  int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  if(pinValue == 1){
//    turnControlLedOn1Sec();
    toggleControlLed();
  }
}

bool controlLedToggle = false;
void toggleControlLed(){
  if(controlLedToggle){
    turnControlLedOn1Sec();
    timer.setTimeout(1000L, turnControlLedOff);
  }
  else{
    turnControlLedOff();
  }
  controlLedToggle != controlLedToggle;
}

void turnControlLedOn1Sec(){
  Serial.println("turnControlLedOn1Sec");
  digitalWrite(D0, HIGH);
  digitalWrite(LED_BUILTIN, LOW);
  timer.setTimeout(1000, turnControlLedOff);
}

void turnControlLedOff(){
  Serial.println("turnControlLedOff");
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(D0, LOW);
}

void setup()
{
  pinMode(D3, OUTPUT);
  digitalWrite(D3, LOW);  
  pinMode(BUILTIN_LED, OUTPUT);  	// set onboard LED as output
  digitalWrite(BUILTIN_LED, HIGH);	// led logic is inverted 
  pinMode(D0, OUTPUT);
  // Debug console
  // Serial.begin(9600);
  
  Serial.begin(115200, SERIAL_8N1, SERIAL_TX_ONLY);
  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8442);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8442);


  irsend.begin();

  //  Force the ESP into client-only mode
  WiFi.mode(WIFI_STA); 

  //  Enable light sleep
  wifi_set_sleep_type(LIGHT_SLEEP_T);
}

void loop()
{
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
}

