#define BLYNK_PRINT Serial
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

int r=2;
int g=14;
int b=12;
const long utcOffsetInSeconds = -19800;

char daysOfTheWeek[7][12] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};

// Define NTP Client to get time
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "asia.pool.ntp.org", utcOffsetInSeconds);

#include "Wire.h"

#include <Arduino.h>
#include <U8x8lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif

U8X8_SH1106_128X64_NONAME_HW_I2C u8x8(/* reset=*/ U8X8_PIN_NONE);

char auth[] = "Ws_AvAufcsxC29PkPNfsA3l9mOycDXSK";
char ssid[] = "Rohans 2G";
char pass[] = "Rohann98";

int temp=0;
int hum=0;
int gas=0;

void rgblow(){
  digitalWrite(r,LOW);
  digitalWrite(g,LOW);
  digitalWrite(b,LOW);
}
void Red_Blink(){
  for(int i=0;i<8;i++){
    digitalWrite(r,HIGH);
    delay(1000);
    digitalWrite(r,LOW);
    delay(1000);
  }
  
}

void Blue_Blink(){
  for(int i=0;i<5;i++){
    digitalWrite(b,HIGH);
    delay(1000);
    digitalWrite(b,LOW);
    delay(1000);
  }
  
}

BLYNK_WRITE(V2) {
   hum = param.asInt();
  Serial.print("Humidity:");
  Serial.println(hum);
  if(hum>90){
    Serial.print("High Humidity");
    u8x8.clearDisplay();
    u8x8.draw1x2String(6,2,"HIGH");
    u8x8.draw1x2String(4,4,"HUMIDITY");
    rgblow();
    Red_Blink();
  }
  else if(hum<30){
    Serial.print("Low Humidity");
    u8x8.clearDisplay();
    u8x8.draw1x2String(6,2,"LOW");
    u8x8.draw1x2String(4,4,"HUMIDITY");
    rgblow();
    Blue_Blink();
    rgblow();
    
  }
  else{
    Serial.println("Normal Conditions");
    rgblow();
     digitalWrite(g,HIGH);
  }
}

BLYNK_WRITE(V1) {
  temp = param.asInt();
  if(temp>20){
    Serial.println("High Temperature");
     u8x8.clearDisplay();
     u8x8.draw1x2String(6,2,"HIGH");
     u8x8.draw1x2String(4,4,"TEMPERATURE");
     rgblow();
     Red_Blink();
     rgblow();  
  }
  else{
    Serial.print("Temperature:");
    Serial.println(temp);
    rgblow();
    digitalWrite(g,HIGH);
  }
}

BLYNK_WRITE(V3) {
  gas = param.asInt();
  if (gas>60){
    Serial.println("Gas Warning");
    u8x8.clearDisplay();
     u8x8.draw1x2String(6,2,"GAS");
     u8x8.draw1x2String(4,4,"WARNING");
     rgblow();
     Red_Blink(); 
     rgblow();
  }
  else{
    Serial.print("GasLevel:");
    Serial.println(gas);
    rgblow();
     digitalWrite(g,HIGH);
  }
}

BLYNK_WRITE(V8){
  int sts = param.asInt();
  if(sts == 1){
    u8x8.clearDisplay();
    u8x8.draw1x2String(6,2,"WEAR");
    u8x8.draw1x2String(5,4,"HELMET");
    Blue_Blink();
    rgblow();
  }
}


void display(){
  u8x8.setFont(u8x8_font_chroma48medium8_r);
  u8x8.draw1x2String(3,1,"HELLO MINER");
  u8x8.draw1x2String(1,3,"WELCOME TO VIT");
  u8x8.draw1x2String(3,5,"MINING INC.");
  for(int i=0;i<5;i++){
    digitalWrite(r,HIGH);
    delay(1000);
    digitalWrite(g,HIGH);
    delay(1000);
    digitalWrite(b,HIGH);
    delay(1000); 
    rgblow();
  }
  
}


void setup(){
  
  
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  pinMode(r, OUTPUT);
  pinMode(g, OUTPUT);
  pinMode(b, OUTPUT);
  u8x8.begin();
  u8x8.setPowerSave(0);
  display();
  timeClient.begin();
  delay(5000);
  
}





void loop()
{
  Blynk.run();
  u8x8.clearDisplay();
  u8x8.drawString(0,2,"TEMPERATURE:");
  u8x8.setCursor(12,2);
  u8x8.print(temp);
  u8x8.print("C");
  u8x8.drawString(0,4,"HUMIDITY:");
  u8x8.setCursor(12,4);
  u8x8.print(hum);
  u8x8.print("%");
  u8x8.drawString(0,6,"GAS LEVEL:");
  u8x8.setCursor(12,6);
  u8x8.print(gas);
  u8x8.print("%");
  u8x8.refreshDisplay();
   timeClient.update();

  Serial.print(daysOfTheWeek[timeClient.getDay()]);
  Serial.print(", ");
  Serial.print(timeClient.getHours()+11);
  Serial.print(":");
  Serial.print(timeClient.getMinutes());
  u8x8.setCursor(6,0);
  u8x8.print(timeClient.getHours()+11);
  u8x8.print(":");
  u8x8.print(timeClient.getMinutes());
  delay(2000);
  
}
