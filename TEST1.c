#include "SPI.h"
#include "WiFi.h"
#include <Servo.h> 
#include <SoftwareSerial.h>

int a = 0;
int b = 0;

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
#endif

char ssid[] = "Bitdol_wifi";   
char pass[] = "bitdol5124!"; 
String location = "Gangnam"; 

WiFiServer server(80);
WiFiClient client;

const unsigned long requestInterval = 60000;

IPAddress hostIp;

uint8_t ret;
unsigned long lastAttemptTime = 0;      
String currentLine = "";       
String tempString = "";       
String humString = "";          
String timeString = "";         
String pressureString = "";    

String weatherString = "";

boolean readingTemp = false;   
boolean readingHum = false;     
boolean readingTime = false;     
boolean readingPressure = false; 

boolean readingWeather = false;

int temp = 0;
int weatherInt;

int      head  = 0, tail = -25; 
uint32_t color = 0xFF0000; 
int i;
int f = 0;
Servo servo; 
int angle = 0;

void setup() {

 
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
 
  currentLine.reserve(100);
  tempString.reserve(10);
  humString.reserve(10);
  timeString.reserve(20);

  weatherString.reserve(30);
  
  Serial.begin(115200);    
  delay(10);
  
  Serial.println("Connecting to WiFi....");  
  WiFi.begin(ssid, pass); 

  server.begin();
  Serial.println("Connect success!");
  Serial.println("Waiting for DHCP address");
  
  while(WiFi.localIP() == INADDR_NONE) {
    Serial.print(".");
    delay(300);
  }
  Serial.println("\n");
  printWifiData();
  connectToServer();

 servo.attach(8); 
 
 pinMode(17, OUTPUT);//A
 pinMode(16, OUTPUT);//B
 pinMode(15, OUTPUT);//C
 pinMode(14, OUTPUT);//D

 pinMode(4, OUTPUT);//C
 pinMode(3, OUTPUT);//D
 pinMode(2, OUTPUT);//A
}

void loop()
{
  if (client.connected()) {
    while (client.available()) {   
      
      char inChar = client.read();    
      currentLine += inChar; 
      
      if (inChar == '\n') {
        //Serial.print("clientReadLine = ");
        //Serial.println(currentLine);
        currentLine = "";
      }

      if(currentLine.endsWith("<weather number=")){
        readingWeather = true;
        weatherString = "";
      }
      if(readingWeather){
        if(inChar != 'v'){
          weatherString += inChar;
        }
        else{
          readingWeather = false;
          Serial.print("- Weather: ");
          Serial.println(getInt(weatherString));

    if(getInt(weatherString)>200&&getInt(weatherString)<300){
   digitalWrite(9, LOW);   
   delay(5);      
   digitalWrite(10, LOW);   
   delay(5);    
  
   
   }

  else if(getInt(weatherString)>=300&&getInt(weatherString)<600){
  digitalWrite(9, LOW);   
   delay(5);      
   digitalWrite(10, HIGH);   
   delay(5);    
  
   }

  else if(getInt(weatherString)>=600&&getInt(weatherString)<782){ 
 digitalWrite(9,HIGH);   
   delay(5);      
   digitalWrite(10, LOW);   
   delay(5);    
  
   }
   
  else if(getInt(weatherString)>783&&getInt(weatherString)<805){ 
     digitalWrite(9, HIGH);   
   delay(5);      
   digitalWrite(10, HIGH);   
   delay(5);    
   Serial.println("eee");
  
  }
  }
  
      }
      
      
      if ( currentLine.endsWith("<temperature value=")) {
        readingTemp = true; 
        tempString = "";
      }      
      if (readingTemp) {
        if (inChar != 'm') { 
          tempString += inChar;
        } 
        else { 
          readingTemp = false;
          Serial.print("-  Temperature: ");
          Serial.print(getInt(tempString)-273);
          Serial.println((char)176); 
          
          a = (getInt(tempString)-273)/10;
          b = (getInt(tempString)-273)%10;
          Serial.println(a); 
          b = b<<2;
          Serial.println(b);
        
          PORTC = (int)a;  
          PORTD = (int)b;
          delay(10000);
        }
      }
      if ( currentLine.endsWith("<humidity value=")) {
        readingHum = true; 
        humString = "";
      }
      if (readingHum) { 
        if (inChar != 'u') {
          humString += inChar;
        } 
        else { 
          readingHum = false;
          Serial.print("-  Humidity: ");
          f = getInt(humString);
          Serial.print(f);
          Serial.println((char)37);
          
        }
      }


      if (readingTime) {
        if (inChar != '/') {
          timeString += inChar;
        }
        else {
          readingTime = false;
          Serial.print("-  Last update: ");
          Serial.println(timeString.substring(2,timeString.length()-1));
        }
      }
       
      if ( currentLine.endsWith("<pressure value=")) {
        readingPressure = true; 
        pressureString = "";
      }
           
      if (readingPressure) {
        if (inChar != 'u') {
          pressureString += inChar;
        } 
        else { 
          readingPressure = false;
          Serial.print("-  Pressure: ");
          Serial.print(getInt(pressureString));
          Serial.println("hPa");

          currentLine = "";
        }
      } 
      
      if ( currentLine.endsWith("</current")) {
        Serial.println("Restart");
        delay(10000); 
        client.stop(); 
        connectToServer();
       }

 
    }
  }   

  else if (millis() - lastAttemptTime > requestInterval) {
    Serial.println("Fail");
    delay(1000);
    client.stop(); 
    connectToServer();
  }  


         if( f >= 75 ){
          angle = 180;
         }else if( f >= 50 ){
          angle = 120;
         Serial.print("120도");
         }else if( f >= 25 ){
          angle = 60;
         }else{
          angle = 0;
         }
         servo.write(angle);
         delay(2000);
  
}


void connectToServer() {
  Serial.println("connecting to server...");
  String content = "";
  if (client.connect(hostIp, 80)) {
    Serial.println("Connected! Making HTTP request to api.openweathermap.org for "+location+"...");
   client.println("GET /data/2.5/weather?id=1835329&APPID=d508cb8bf7640cc3aa872218d41760e2&mode=xml");
    client.print("HOST: api.openweathermap.org\n");
    client.println("Connection: close");
    client.println();
    Serial.println("Weather information for "+location);
   }
  lastAttemptTime = millis();
  Serial.println(lastAttemptTime);
}

void printHex(int num, int precision) {
  char tmp[16];
  char format[128];
  sprintf(format, "%%.%dX", precision);
  sprintf(tmp, format, num);
  Serial.print(tmp);
}

void printWifiData() {
  Serial.println();
  Serial.println("IP Address Information:");  
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  byte mac[6];  
  WiFi.macAddress(mac);
  Serial.print("MAC address: ");
  printHex(mac[5], 2);
  Serial.print(":");
  printHex(mac[4], 2);
  Serial.print(":");
  printHex(mac[3], 2);
  Serial.print(":");
  printHex(mac[2], 2);
  Serial.print(":");
  printHex(mac[1], 2);
  Serial.print(":");
  printHex(mac[0], 2);
  Serial.println();
  IPAddress subnet = WiFi.subnetMask();
  Serial.print("NetMask: ");
  Serial.println(subnet);
  IPAddress gateway = WiFi.gatewayIP();
  Serial.print("Gateway: ");
  Serial.println(gateway);
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  ret = WiFi.hostByName("api.openweathermap.org", hostIp);
  Serial.print("ret: ");
  Serial.println(ret);
  Serial.print("Host IP: ");
  Serial.println(hostIp);
  Serial.println("");
}

int getInt(String input){
  int i = 2;
  while(input[i] != '"'){
    i++;
  }
  input = input.substring(2,i);
  char carray[20];
  input.toCharArray(carray, sizeof(carray));
  temp = atoi(carray);
  return temp;
}
