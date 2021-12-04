//Including the libraries
#define BLYNK_PRINT Serial
#include <TinyGPS++.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include<SoftwareSerial.h>

#include "Wire.h"

//Gps Module pins
static const int RXPin = 0, TXPin = 2;   // Rx -> D3 and Tx-> D4
static const uint32_t GPSBaud = 9600; 


TinyGPSPlus gps; // The TinyGPS++ object
WidgetMap myMap(V0);  // V0 for virtual pin of Map Widget
SoftwareSerial ss(RXPin, TXPin);


//Defining the pins
#define DHTPIN 12 //D6 pin of Node MCU 
#define DHTTYPE DHT11 // Type of DHT sensor
#define Sensor A0 //Gas sensor data
# define b 15 // Buzzer D8



//Auth Token in the Blynk App.
char auth[] = "Ws_AvAufcsxC29PkPNfsA3l9mOycDXSK";

//Network parameters
char ssid[] = "Rohans 2G";
char pass[] = "Rohann98";

//Initializing the parameters
int value = 200;
int sensor = 0; // Gas sensor value
int panicPin = 14; //Pin D4 of panic button
int panicVal= 0; //Panic button value
int Irpin = 13; //Helmet remove status pin
int ir =0; //Helmet remove status


//Declaring the objects
DHT dht(DHTPIN, DHTTYPE);//DHT 11
WidgetLCD lcd(V4);//Blynk app LCD
BlynkTimer timer; //Timer 0
BlynkTimer timer1; //Timer 1
WidgetBridge bridge1(V9); //Blynk Bridge

//For the Blynk Bridge
BLYNK_CONNECTED() {
  bridge1.setAuthToken("Ws_AvAufcsxC29PkPNfsA3l9mOycDXSK"); 
}


float h , t; //Variables for storing humidity and temperature
float spd;       //Variable  to store the speed
float sats;      //Variable to store no. of satellites response
String bearing;  //Variable to store orientation or direction of GP       
unsigned int move_index = 1;  // moving index

//To send the Humidity and temperature data
void sendSensor()
{
 h = dht.readHumidity(); // Reading Humidity
 t = dht.readTemperature(); //Reading Temperature
 
 sensor = analogRead(Sensor);
 sensor = map(sensor,0,1024,0,100);

 //Checking whether sensors are giving output
  if (isnan(h) || isnan(t)|| isnan(sensor)||isnan(Irpin)) {
    Serial.println("Failed to read from sensor!");
    lcd.clear();
    lcd.print(4,0,"Connection"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
    lcd.print(4,1,"Error");
    delay(1000);
    return;
  }
  //Sending data to the Blynk server
  //Humidity
  Blynk.virtualWrite(V2, h);
  bridge1.virtualWrite(V2,h);
  //Temperature
  Blynk.virtualWrite(V1, t);
  bridge1.virtualWrite(V1,t);
  //Gas Sensor data
  Blynk.virtualWrite(V3, sensor);  
  bridge1.virtualWrite(V3,sensor);
}

//Panic Button Functions
void PanicButton(){
  lcd.clear();
  lcd.print(4,0,"Emergency"); 
  lcd.print(3,1,"Please Help");
  Blynk.virtualWrite(V7,HIGH);
  
  delay(100);
}


void setup()
{
  Serial.begin(115200);
  Serial.println();
  
  ss.begin(GPSBaud);
  delay(100);
  //setting the pin modes
  pinMode(b,OUTPUT);
  pinMode(Sensor,INPUT);
  pinMode(panicPin ,INPUT);
  pinMode(Irpin, INPUT);

  //Starting DHT11
  dht.begin();

  //Initializing Blynk
  Blynk.begin(auth, ssid, pass);
 
  timer.setInterval(5000L, checkGPS); // Checking GPS every 5sec
  timer1.setInterval(2000L, sendSensor); //Checking Sensor values every 2sec

  
}

//Buzzer sound 2
void BuzzerBeep1(){
  digitalWrite(b,HIGH);
  delay(1);
  digitalWrite(b,LOW);
  delay(1);
  digitalWrite(b,HIGH);
  delay(1);
}

//Buzzer Sound 1
void BuzzerBeep(){
  for(int i=0;i<100;i++){
    digitalWrite(b,HIGH);
    delay(2);
    digitalWrite(b,LOW);
    delay(2);
  }
}

//Starting Buzzzer
void Buzzer(){
  digitalWrite(b,HIGH);
}

//Stopping Buzzer
void Buzzerstop(){
  digitalWrite(b,LOW);
}

//Checking GPS module is sending data or not
void checkGPS(){
  if (gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));  // Value Display widget  on V4 if GPS not detected
  }
}


void loop()
{
  //Gps Functions
  while (ss.available() > 0) 
  {
    // sketch displays information every time a new sentence is correctly encoded.
    if (gps.encode(ss.read())){
      Serial.println("GPS INITIATED");
      //Calling the GPS function to display
      displayInfo();
    }
  }
  //Reading the helmet state
  ir = digitalRead(Irpin);

  //Checking the Helmet remove condition
  if(ir == 1)
  {
    
    Blynk.virtualWrite(V8,HIGH);
    bridge1.virtualWrite(V8,HIGH);
   
    BuzzerBeep();
    lcd.clear();
    lcd.print(4,0,"Helmet "); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
    lcd.print(4,1,"Removed");
    delay(1000);
    
  }
  else{
    digitalWrite(b,LOW);
    
  }
  
  Blynk.run();
  timer1.run();
  
  

  //Panic Button Functions
  panicVal = digitalRead(panicPin);
  if(panicVal == LOW){
    BuzzerBeep1();
    Blynk.email("rohanpaul060598@gmail.com","Emergency","Please Help");//Emailing the authorities
    PanicButton();
    lcd.clear();
    Buzzerstop();
  }
  panicVal = LOW; 

  //Reading Temperature and Humidity
  h = dht.readHumidity();
  t = dht.readTemperature();

  //Checking temperature conditions
  if (t>=30)        
  { 
    Serial.print("WARNING HIGH TEMPERATURE");
    lcd.clear();
    lcd.print(4,0,"High"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
    lcd.print(4,1,"Temperature");
    delay(2000);
    Serial.print(t); 
   
    BuzzerBeep();
  }
  else{
    //Serial.print("CONDITIONS ARE NORMAL");
    lcd.clear();
    lcd.print(4, 0, "Normal"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
    lcd.print(2, 1, "Conditions");
    
    Buzzerstop();
  }
  delay(100);

  //Checking the Humidity Conditions
  if (h<30)        
  { 
    Serial.println("LOW HUMIDITY");
    Serial.print(h);
    lcd.clear();    
    lcd.print(4, 0, "Low"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
    lcd.print(4, 1, "Humidity");
  
    BuzzerBeep();
    delay(2000);
  }
  else if(h>95)                
  {
    Serial.println("HIGH HUMIDITY");
    Serial.print(h);
    lcd.clear();    
    lcd.print(4, 0, "High"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
    lcd.print(4, 1, "Humidity");
  
    BuzzerBeep();
  }
  else{
    Buzzerstop();
    
  }

  //Checking gas sensor parameter
  if(sensor>54){
   
    Serial.print("GAS ALERT");
    lcd.clear();
    lcd.print(1, 0, "Gas Alert"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
    delay(2000);
    BuzzerBeep();
  }
  else{
    Buzzerstop();
  }
  
  delay(800);
}

//To display the gps module data
void displayInfo()
{ 
  //Checking the latitude and longitude values
  float latitude = (gps.location.lat());     //Storing the Lat. and Lon. 
  float longitude = (gps.location.lng()); 

  
  Serial.print("LAT:  ");
  Serial.println(latitude, 6);  // float to x decimal places
  Serial.print("LONG: ");
  Serial.println(longitude, 6);
  
  Blynk.virtualWrite(V5, String(latitude, 6));   
  Blynk.virtualWrite(V6, String(longitude, 6));   
  myMap.location(move_index, latitude, longitude, "GPS_Location");
  spd = gps.speed.kmph();               //get speed
       Serial.print("SPEED:");
       Serial.println(spd);
       
       sats = gps.satellites.value();    //get number of satellites
       Serial.print("NUMBER OF SATELLITES:");
       Serial.println(sats);

       bearing = TinyGPSPlus::cardinal(gps.course.value()); // get the direction
       Serial.print("DIRECTION:");
       Serial.println(bearing);    
  
 Serial.println();
}
