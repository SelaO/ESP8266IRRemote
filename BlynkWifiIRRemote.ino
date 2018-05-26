char auth[] = "";
char ssid[] = "";
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
  */
  
/* Comment this out to disable prints and save space */
#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG
#define BLYNK_NO_BUILTIN   // Disable built-in analog & digital pin operations
#define BLYNK_NO_FLOAT     // Disable float operations
#define TIMEZONE_OFFSET 2

#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <ESP8266WiFi.h> 
#include <BlynkSimpleEsp8266.h> // don't use SSL on the wemos D1 mini since it's too weak
#include <NTPClient.h>
#include <WiFiUdp.h>
extern "C" {
  #include "user_interface.h"
}

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP); // to get the real time
BlynkTimer timer;

IRsend irsend(D3); 
int khz = 38;
uint16_t ACOnColdCode[] = {8936,4440,704,1576,716,480,656,536,660,1624,656,1624,656,1628,652,540,660,536,656,1624,660,1624,656,540,652,540,660,536,656,536,656,540,652,540,652,544,660,536,656,536,656,540,652,1628,652,1632,660,1620,660,536,656,536,656,540,652,544,660,532,656,1628,652,540,652,1632,652,540,660,536,656,1624,656,540,652,19836,656,536,656,540,652,540,652,544,660,532,688,508,684,508,684,512,680,516,688,504,688,508,684,508,684,512,680,1600,660,536,688,504,688,512,680,512,680,512,688,508,684,508,684,512,680,520,684,504,688,504,688,508,684,512,680,512,692,504,688,504,688,508,684,1596,684};
uint16_t ACOffCode[] = {8964,4412,660,1624,656,536,656,540,656,540,648,1632,660,1624,660,532,660,536,688,1592,688,1596,652,540,684,512,680,516,688,504,688,508,684,508,684,512,680,512,684,512,688,508,688,504,688,1596,684,1596,684,512,680,512,692,504,688,504,688,508,684,1596,688,508,684,1600,680,512,680,516,688,1592,688,508,684,19800,688,508,684,512,680,512,680,516,688,504,688,508,684,508,684,512,680,512,680,516,676,516,688,508,684,512,680,1600,684,512,680,512,688,508,684,508,688,508,684,508,684,512,680,516,688,504,688,508,684,508,684,512,680,516,688,504,688,508,684,508,684,512,680,512,680};
uint16_t ACOnHotCode[] = {};
uint16_t FanToggleCode[] = {1324,356,1304,376,440,1248,1272,408,1272,416,432,1260,440,1256,444,1252,492,1208,440,1256,440,1252,1280,6968,1272,404,1304,376,476,1212,1304,376,1304,384,468,1228,440,1256,444,1252,436,1260,440,1256,444,1252,1308,6920,1308,372,1300,380,468,1220,1300,380,1300,388,472,1220,468,1228,472,1224,476,1224,464,1232,468,1228,1300,6944,1308,368,1300,380,472,1216,1300,380,1300,388,472,1224,464,1232,468,1228,472,1224,468,1228,472,1224,1304,6952,1300,376,1300,380,472,1216,1300,380,1300,388,464,1228,472,1224,476,1220,468,1232,468,1228,472,1220,1308};
uint16_t BiasLightOn[] = {9150,4400, 600,550, 600,500, 600,550, 600,550, 600,500, 600,550, 600,500, 650,500, 600,1650, 550,1650, 600,1600, 600,1650, 600,550, 600,1600, 650,1550, 650,1600, 650,1550, 650,1600, 600,500, 650,500, 650,500, 600,500, 650,500, 600,500, 650,500, 650,500, 600,1600, 650,1550, 650,1600, 600,1600, 650,1600, 600,1600, 650};
uint16_t BiasLightOff[] = {9150,4400, 600,500, 600,550, 600,550, 600,500, 600,550, 600,500, 600,550, 600,550, 600,1600, 600,1650, 600,1600, 600,1600, 650,500, 650,1600, 600,1600, 650,1550, 650,500, 650,1550, 650,500, 650,500, 600,500, 650,500, 600,500, 650,500, 650,1600, 600,500, 650,1600, 600,1600, 650,1600, 600,1600, 650,1550, 650,1600, 650};

bool preventBiasLightToday = false;
int lastBiasLightTimerId = -1;

void SendIRCommand(uint16_t* IRCommandArray, uint16_t len){
  irsend.sendRaw(IRCommandArray, len / sizeof(IRCommandArray[0]), khz); 
  // indicate on control led that a transmission is ongoing 
  turnControlLedOn1Sec();
}

void sendLastCommandStringBack(String command, int timeout = 5000){
  Blynk.virtualWrite(V11, command);
  timer.setTimeout(timeout, [](){
    Blynk.virtualWrite(V11, "---");
  });
}

/// region bias light

void CheckTurnBiasLightOn(){
  timeClient.update();
  int currHour = (timeClient.getHours() + TIMEZONE_OFFSET) % 24;
  
  if(currHour > 6 && currHour < 22 && !preventBiasLightToday){
    SendIRCommand(BiasLightOn, sizeof(BiasLightOn));  
  }
}

void preventBiasRelight(){
   preventBiasLightToday = false;
   lastBiasLightTimerId = -1;
}

void toggleBiasLight() {
  Serial.print("BiasLight");
  if(lastBiasLightTimerId >= 0){
    timer.deleteTimer(lastBiasLightTimerId);
  }
  if(preventBiasLightToday){
    Serial.println(" on");
    SendIRCommand(BiasLightOn, sizeof(BiasLightOn)); 
  }
  else{
    Serial.println(" off");
    lastBiasLightTimerId = timer.setTimeout(21600000, preventBiasRelight);
    SendIRCommand(BiasLightOff, sizeof(BiasLightOff));  
  }
  preventBiasLightToday = !preventBiasLightToday;
  sendLastCommandStringBack("BiasLight toggle");
}

// BiasLight
BLYNK_WRITE(V7)
{
  int pinValue = param.asInt(); 
  if(pinValue == 1){
    toggleBiasLight();
  }
}

/// endregion bias light

// ACOnCold
BLYNK_WRITE(V1)
{
  int pinValue = param.asInt(); 
  if(pinValue == 1){
    //ACOnCold();
    timer.setTimer(500, ACOnCold, 3);
  }
}

// enter area
BLYNK_WRITE(V5)
{
  Serial.println("enter area");
  sendLastCommandStringBack("enter area");
  int pinValue = param.asInt(); 
  if(pinValue == 1){
    timer.setTimer(500, ACOnCold, 3);
    sendLastCommandStringBack("enter area", 600000);
  }
}

void ACOnCold(){  // todo to be refactored 
  Serial.println("ACOnCold");
  SendIRCommand(ACOnColdCode, sizeof(ACOnColdCode));
  sendLastCommandStringBack("ACOnCold");
}

// ACOff
BLYNK_WRITE(V2)
{
  int pinValue = param.asInt(); 
  if(pinValue == 1){
    SendIRCommand(ACOffCode, sizeof(ACOffCode));
    Serial.println("ACOff");
    sendLastCommandStringBack("ACOff");
  }
}

// exit area
BLYNK_WRITE(V8)
{
  Serial.println("exit area");
  int pinValue = param.asInt(); 
  if(pinValue == 1){
    SendIRCommand(ACOffCode, sizeof(ACOffCode));
    Serial.println("ACOff");
    sendLastCommandStringBack("ACOff", 600000);
  }
}

// ACOnHot
BLYNK_WRITE(V3)
{
  int pinValue = param.asInt(); 
  if(pinValue == 1){
    SendIRCommand(ACOnHotCode, sizeof(ACOnHotCode));
    Serial.println("ACOnHot");
  }
}

// fanToggle
BLYNK_WRITE(V4)
{
  int pinValue = param.asInt(); 
  if(pinValue == 1){
    SendIRCommand(FanToggleCode, sizeof(FanToggleCode));
    Serial.println("fanToggle");
  }
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
  digitalWrite(LED_BUILTIN, LOW);
  timer.setTimeout(1000, turnControlLedOff);
}

void turnControlLedOff(){
  Serial.println("turnControlLedOff");
  digitalWrite(LED_BUILTIN, HIGH);
}

void setup()
{
  // Serial.println(ESP.getResetInfo());
  // Serial.println(ESP.getResetInfoPtr());
  pinMode(D3, OUTPUT);
  digitalWrite(D3, LOW);  
  pinMode(BUILTIN_LED, OUTPUT);  	// set onboard LED as output
  digitalWrite(BUILTIN_LED, HIGH);	// led logic is inverted 
  // Debug console
  Serial.begin(9600, SERIAL_8N1, SERIAL_TX_ONLY);

  Blynk.begin(auth, ssid, pass);
  // You can also specify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8442);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8442);

  irsend.begin();

  // Force the ESP into client-only mode
  WiFi.mode(WIFI_STA); 

  // Enable light sleep
  // wifi_set_sleep_type(LIGHT_SLEEP_T);

  timeClient.begin(); // to turn on the bias light in the morning automatically
  CheckTurnBiasLightOn();
  timer.setTimer(1800000, CheckTurnBiasLightOn, 1);

  // timer.setInterval(1000L, &debugFunc);
  // Blynk.syncAll();
}

void debugFunc(){
    Blynk.virtualWrite(V10, timer.getNumTimers());  // Displaying the # of active timers named "timer"
    Serial.print("getNumTimers: ");
    Serial.println(timer.getNumTimers());  // Printing the # of active timers named "timer"
    Serial.print("heap: ");
    Serial.println(ESP.getFreeHeap());
    Serial.print("resetInfo: ");
    Serial.println(ESP.getResetInfo());
    Serial.println("---");
}

void loop()
{
  Blynk.run();
  timer.run(); // Initiates BlynkTimer
  
}

