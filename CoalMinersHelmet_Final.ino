#define BLYNK_PRINT Serial
//#include <TinyGPS++.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include<SoftwareSerial.h>


  

//static const int RXPin = 4, TXPin = 5;   // GPIO 4=D2(conneect Tx of GPS) and GPIO 5=D1(Connect Rx of GPS
//static const uint32_t GPSBaud = 9600; 

//TinyGPSPlus gps; // The TinyGPS++ object
//WidgetMap myMap(V0);  // V0 for virtual pin of Map Widget
//SoftwareSerial ss(RXPin, TXPin);

#define DHTPIN 14 //D5 pin of Node MCU 
#define DHTTYPE DHT11
#define R1 12 //Buzzer
#define Sensor A0 //Gas sensor data


// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "eYeMa5W94qSfoP_bSGxVVbm962DMTghJ";

// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] = "Rohans 2G";
char pass[] = "Rohann98";

int value = 200;
int sensor = 0;
int panicPin = 2;
int panicVal= 0;



DHT dht(DHTPIN, DHTTYPE);
WidgetLCD lcd(V4);
//BlynkTimer timer;
BlynkTimer timer1;

float h , t; //Variables for storing humidity and temperature
//unsigned int move_index;         // moving index, to be used later
unsigned int move_index = 1;       // fixed location for now


void sendSensor()
{
 h = dht.readHumidity();
 t = dht.readTemperature(); // or dht.readTemperature(true) for Fahrenheit
 sensor = analogRead(Sensor);
 sensor = map(sensor,0,1024,0,100);
  if (isnan(h) || isnan(t)|| isnan(sensor)) {
    Serial.println("Failed to read from sensor!");
    return;
  }
  Blynk.virtualWrite(V2, h);
  Blynk.virtualWrite(V1, t);
  Blynk.virtualWrite(V3, sensor);  
}



void setup()
{
  // Debug console
  Serial.begin(115200);
  Serial.println();
//  ss.begin(GPSBaud);
  delay(100);
  pinMode(R1,OUTPUT);
  pinMode(Sensor,INPUT);
  pinMode(panicPin ,INPUT);
  digitalWrite(R1,0);
  
  dht.begin();

  Blynk.begin(auth, ssid, pass);
  timer1.setInterval(2000L, sendSensor);
//  timer.setInterval(5000L, checkGPS);
  
}

//void checkGPS(){
//  if (gps.charsProcessed() < 10)
//  {
//    Serial.println(F("No GPS detected: check wiring."));  // Value Display widget  on V4 if GPS not detected
//  }
//}

void loop()
{
//  while (ss.available() > 0) 
//  {
//    // sketch displays information every time a new sentence is correctly encoded.
//    if (gps.encode(ss.read())){
//      Serial.println("GPS INITIATED");
//      displayInfo();
//    }
//  }
  Blynk.run();
  timer1.run();
  
  
  h = dht.readHumidity();
  t = dht.readTemperature();
  
  if (t>48)        
  { 
    Serial.print("WARNING HIGH TEMPERATURE");
    lcd.clear();
    lcd.print(4,0,"High"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
    lcd.print(4,1,"Temperature");
    delay(2000);
    Serial.print(t); 
    digitalWrite(R1,HIGH);  
  }
  else{
    Serial.print("CONDITIONS ARE NORMAL");
    lcd.clear();
    lcd.print(4, 0, "Normal"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
    lcd.print(4, 1, "Conditions");
  }
  delay(100);
  if (h<30)        
  { 
    Serial.println("LOW HUMIDITY");
    Serial.print(h);
    lcd.clear();    
    lcd.print(4, 0, "Low"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
    lcd.print(4, 1, "Humidity");
    delay(2000);
  }
  else if(h>80)                
  {
    Serial.println("HIGH HUMIDITY");
    Serial.print(h);
    lcd.clear();    
    lcd.print(4, 0, "High"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
    lcd.print(4, 1, "Humidity");
  }

  if(sensor>64){
    digitalWrite(R1,HIGH);
    Serial.print("GAS ALERT");
    lcd.clear();
    lcd.print(1, 0, "Gas Alert"); // use: (position X: 0-15, position Y: 0-1, "Message you want to print")
    delay(2000);
  }
  
  delay(800);
}

//void displayInfo()
//{ 
//  if (gps.location.isValid() ) 
//  {    
//    float latitude = (gps.location.lat());     //Storing the Lat. and Lon. 
//    float longitude = (gps.location.lng()); 
//    
//    Serial.print("LAT:  ");
//    Serial.println(latitude, 6);  // float to x decimal places
//    Serial.print("LONG: ");
//    Serial.println(longitude, 6);
//     
//    myMap.location(move_index, latitude, longitude, "GPS_Location");
//                   
//  }
//  
// Serial.println();
//}
