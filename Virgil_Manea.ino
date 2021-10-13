
// Include DHT library and Adafruit Sensor Library
#include "DHT.h"
#include <Adafruit_Sensor.h>
//Include Software Serial library to communicate with GSM
#include <SoftwareSerial.h>

// Pin DHT is connected to
#define DHTPIN 2
  
// Uncomment whatever type of sensor you're using
#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Initialize DHT sensor for normal 16mhz Arduino
DHT dht(DHTPIN, DHTTYPE);

// Create global varibales to store temperature and humidity
float t; // temperature in celcius
float f; // temperature in fahrenheit
float h; // humidity

int Alarm = 5;
int switch_ON_Flag_previous_I = 0;

unsigned long previousMillis = 0;       

// constants won't change:
const long interval = 5*60*60;
int i = 0;            


// Configure software serial port
SoftwareSerial SIM900(7, 8);

// Create variable to store incoming SMS characters
char incomingChar;

void setup() {
  dht.begin();
  
  Serial.begin(19200); 
  SIM900.begin(19200);

 

  // Give time to your GSM shield log on to network
  delay(20000);
  Serial.print("SIM900 ready...");

  // AT command to set SIM900 to SMS mode
  SIM900.print("AT+CMGF=1\r"); 
  delay(100);
  // Set module to send SMS data to serial out upon receipt 
  SIM900.print("AT+CNMI=2,2,0,0,0\r");
  delay(100);
if(readData()){
      delay(10);
      String dataMessage = ("Temperature: " + String(t) + "*C " + " Humidity: " + String(h) + "%");
      sendSMS(dataMessage); 
    }
    }

void loop(){
if(readData()){
   if(t>=25) {
    if (switch_ON_Flag_previous_I == 0 ){
     // REPLACE WITH YOUR OWN SMS MESSAGE CONTENT
      String dataMessage = ("High Temperature: " + String(t) + "*C " + "%");
      sendSMS(dataMessage);
     digitalWrite(Alarm, HIGH);
     switch_ON_Flag_previous_I = 1;}
     
      if(SMSRequest()){
      digitalWrite(Alarm, LOW);
                      }
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval) {
        if(i<=3){
         String dataMessage = ("High Temperature: " + String(t) + "*C " + "%");
         sendSMS(dataMessage);
         digitalWrite(Alarm, HIGH);
          }
          if(i>3){
            

            delay(10);
      // REPLACE THE X's WITH THE RECIPIENT'S MOBILE NUMBER
      // USE INTERNATIONAL FORMAT CODE FOR MOBILE NUMBERS
      SIM900.println("AT + CMGS = \"+XXXXXXXXXX\"");//Add secondary no. in this section.
      delay(100);
      String dataMessage = ("High Temperature: " + String(t) + "*C " + "%");
      SIM900.print(dataMessage);
      delay(100);
      // End AT command with a ^Z, ASCII code 26
      SIM900.println((char)26); 
      delay(100);
      SIM900.println();
      // Give module time to send SMS
      delay(5000); 
      }
         previousMillis = currentMillis;
         i=i+1;
          }
         }
  delay(10); 
}
}
boolean readData() {
  //Read humidity
  h = dht.readHumidity();
  // Read temperature as Celsius
  t = dht.readTemperature();
  // Read temperature as Fahrenheit
  f = dht.readTemperature(true);

  // Compute temperature values in Celcius
  t = dht.computeHeatIndex(t,h,false);

  // Uncomment to compute temperature values in Fahrenheit
  //f = dht.computeHeatIndex(f,h,false);
  
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return false;
  }
  Serial.print("Humidity: "); 
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: "); 
  Serial.print(t);
  Serial.print(" *C ");
  //Uncomment to print temperature in Farenheit
  //Serial.print(f);
  //Serial.print(" *F\t");
  return true;
}
boolean SMSRequest() {
  if(SIM900.available() >0) {
    incomingChar=SIM900.read();
    if(incomingChar=='O') {
      delay(10);
      Serial.print(incomingChar);
      incomingChar=SIM900.read();
      if(incomingChar =='K') {
        delay(10);
        Serial.print(incomingChar);
        Serial.print("...Response Received \n");
            return true;
            }
          }
        }
      return false;
     }

     // Function that sends SMS
void sendSMS(String dataMessage){
      delay(10);
      // REPLACE THE X's WITH THE RECIPIENT'S MOBILE NUMBER
      // USE INTERNATIONAL FORMAT CODE FOR MOBILE NUMBERS
      SIM900.println("AT + CMGS = \"+XXXXXXXXXX\"");//Add primary no in this section
     
      delay(100);     
                                // Send the SMS text message
      SIM900.print(dataMessage);
      delay(100);
      // End AT command with a ^Z, ASCII code 26
      SIM900.println((char)26); 
      delay(100);
      SIM900.println();
      // Give module time to send SMS
      delay(5000);  
      }
