#include "SPI.h"
#include "WiFi.h"
//#include <Adafruit_DotStar.h>
#include <Servo.h> 
#include <SoftwareSerial.h>

//#define NUMPIXELS 26

//#define DATAPIN    9
//#define CLOCKPIN   10

//Adafruit_DotStar strip = Adafruit_DotStar(NUMPIXELS, DATAPIN, CLOCKPIN, DOTSTAR_BRG);

int a = 0;
int b = 0;

#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000L)
  clock_prescale_set(clock_div_1); // Enable 16 MHz on Trinket
#endif

char ssid[] = "Bitdol_wifi";       //와이파이 SSID
char pass[] = "bitdol5124!";   //와이파이 password 
String location = "Gangnam"; //날씨정보를 보고싶은 지역

//인스턴스 변수 초기화

WiFiServer server(80);
WiFiClient client;

const unsigned long requestInterval = 60000;  // 요구 시간 딜레이(1 min)

IPAddress hostIp;

uint8_t ret;
unsigned long lastAttemptTime = 0;            // 마지막으로 서버에서 데이터를 전송받은 시간
String currentLine = "";          // 서버에서 전송된 데이터 String저장
String tempString = "";           // 온도 저장 변수
String humString = "";            // 습도 저장 변수
String timeString = "";           // 시간 정보 변수
String pressureString = "";       // 압력 정보 변수

String weatherString = "";

boolean readingTemp = false;      //온도 데이터가 있는지 여부 판단
boolean readingHum = false;       //습도 데이터가 있는지 여부 판단
boolean readingTime = false;      //시간 데이터가 있는지 여부 판단
boolean readingPressure = false;  //압력 데이터가 있는지 여부 판단

boolean readingWeather = false;

int temp = 0;
int weatherInt;

int      head  = 0, tail = -25; // Index of first 'on' and 'off' pixels
uint32_t color = 0xFF0000;        // 'On' color (starts red)
int i;
int f = 0;
Servo servo; 
int angle = 0;

void setup() {


pinMode(9, OUTPUT);
pinMode(10, OUTPUT);
 
  //각 변수에 정해진 공간 할당
  currentLine.reserve(100);
  tempString.reserve(10);
  humString.reserve(10);
  timeString.reserve(20);

  weatherString.reserve(30);
  
  Serial.begin(115200);    
  delay(10);
  //WiFi연결 시도
  Serial.println("Connecting to WiFi....");  
  WiFi.begin(ssid, pass);  //WiFi가 패스워드를 사용한다면 매개변수에 password도 작성

  server.begin();
  Serial.println("Connect success!");
  Serial.println("Waiting for DHCP address");
  //DHCP주소를 기다린다
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


// pinMode(3, OUTPUT);//B
 pinMode(4, OUTPUT);//C
 pinMode(3, OUTPUT);//D
 pinMode(2, OUTPUT);//A
}

void loop()
{
  if (client.connected()) {
    while (client.available()) {        //전송된 데이터가 있을 경우 데이터를 읽어들인다.
      
      
      char inChar = client.read();      // 읽어온 데이터를 inChar에 저장한다.
      

      currentLine += inChar;            //inChar에 저장된 Char변수는 currentLine이라는 String변수에 쌓이게 된다.
      //라인피드(줄바꿈)문자열이 전송되면 데이터를 보내지 않는다.
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

    if(getInt(weatherString)>200&&getInt(weatherString)<300){ // 번개
   digitalWrite(9, LOW);   
   delay(5);      
   digitalWrite(10, LOW);   
   delay(5);    
  
   
   }

  else if(getInt(weatherString)>=300&&getInt(weatherString)<600){ // 비
  digitalWrite(9, LOW);   
   delay(5);      
   digitalWrite(10, HIGH);   
   delay(5);    
  
   }

  else if(getInt(weatherString)>=600&&getInt(weatherString)<782){ // 눈
 digitalWrite(9,HIGH);   
   delay(5);      
   digitalWrite(10, LOW);   
   delay(5);    
  
   }
   
  else if(getInt(weatherString)>783&&getInt(weatherString)<805){ // 맑음
     digitalWrite(9, HIGH);   
   delay(5);      
   digitalWrite(10, HIGH);   
   delay(5);    
   Serial.println("eee");
  
  }
  }
  
      }
      
      //온도 데이터가 전송되었는지 확인
      if ( currentLine.endsWith("<temperature value=")) {
        //현재 스트링이 "<temperature value="로 끝났다면 온도데이터를 받을 준비를 한다.
        readingTemp = true; 
        tempString = "";
      }      
      //<temperature value=뒤에 오는 문자열을 tempString에 저장한다.
      if (readingTemp) {
        if (inChar != 'm') { //전송될 문자가 'm'이 올때까지 온도값으로 인식
          tempString += inChar;
        } 
        else { //전송된 문자가 'm'이라면 온도데이터를 그만 저장하고 온도값 출력
          readingTemp = false;
          Serial.print("-  Temperature: ");
          Serial.print(getInt(tempString)-273);
          Serial.println((char)176);    //degree symbol

          a = (getInt(tempString)-273)/10;
          b = (getInt(tempString)-273)%10;
          Serial.println(a); 
          b = b<<2;
          Serial.println(b);
        
          PORTC = (int)a;  
          //PORTD=9;
          PORTD = (int)b;
          delay(10000);
        }
      }
      if ( currentLine.endsWith("<humidity value=")) {
        //현재 스트링이 "<humidity value ="로 끝났다면 습도 데이터를 받을 준비를 한다.
        readingHum = true; 
        humString = "";
      }
      if (readingHum) { 
        if (inChar != 'u') {//전송될 문자열이 'u'가 아니라면 계속 습도값을 받게 된다.
          humString += inChar;
        } 
        else { //다음에 전송된 문자열이 'u'라면 습도값을 그만 받고 값을 출력한다.
          readingHum = false;
          Serial.print("-  Humidity: ");
          f = getInt(humString);
          Serial.print(f);
          Serial.println((char)37);
          
        }
      }


      if (readingTime) {
        if (inChar != '/') { //다음 전송될 문자가 '/'가 아니라면 계속적으로 시간데이터를 받는다
          timeString += inChar;
        }
        else {
          readingTime = false;
          Serial.print("-  Last update: ");
          Serial.println(timeString.substring(2,timeString.length()-1));
        }
      }
       
      if ( currentLine.endsWith("<pressure value=")) {
        // 현재 스트링이 "<pressure value="로 끝났다면 기압 데이터를 받을 준비를 한다.
        readingPressure = true; 
        pressureString = "";
      }
           
      if (readingPressure) {
        if (inChar != 'u') { //다음 전송될 문자가 'u'가 아니라면 계속 기압데이터를 받는다.
          pressureString += inChar;
        } 
        else { //다음 전송된 문자가 'u'라면 기압데이터를 출력한다.

          readingPressure = false;
          Serial.print("-  Pressure: ");
          Serial.print(getInt(pressureString));
          Serial.println("hPa");

          currentLine = "";
        }
      } //pressure 없애면 weather가 안뜸! 이유는 몰게땅ㅇ

      
      if ( currentLine.endsWith("</current")) { //현재 스트링이 </current>로 끝났다면 연결을 끊고 다시 서버와 연결을 준비한다.
        Serial.println("Restart");
        delay(10000); //10초뒤에 서버와 연결을 끊고 재연결을 시도한다.
        client.stop(); 
        connectToServer();
        //Serial.println("Disconnected from Server.\n");
      }

 
    }
  }   

  else if (millis() - lastAttemptTime > requestInterval) {
    //연결을 실패했다면 requestInterval(60초)이후에 다시 연결을 시도한다.
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

//서버와 연결

void connectToServer() {
  Serial.println("connecting to server...");
  String content = "";
  if (client.connect(hostIp, 80)) {
    Serial.println("Connected! Making HTTP request to api.openweathermap.org for "+location+"...");
    //Serial.println("GET /data/2.5/weather?q="+location+"&mode=xml");
   client.println("GET /data/2.5/weather?id=1835329&APPID=d508cb8bf7640cc3aa872218d41760e2&mode=xml");
   //client.println("GET /data/2.5/weather?q="+location+"&mode=xml");
    //위에 지정된 주소와 연결한다.
    client.print("HOST: api.openweathermap.org\n");
    //client.println("User-Agent: launchpad-wifi");
    client.println("Connection: close");
    client.println();
    Serial.println("Weather information for "+location);
   }
  //마지막으로 연결에 성공한 시간을 기록
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
  // Wifi쉴드의 IP주소를 출력
  Serial.println();
  Serial.println("IP Address Information:");  
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  //MAC address출력
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
  //서브넷 마스크 출력
  IPAddress subnet = WiFi.subnetMask();
  Serial.print("NetMask: ");
  Serial.println(subnet);
  //게이트웨이 주소 출력
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



int getInt(String input){  //String데이터를 intger형으로 변환하는 함수
  int i = 2;
  while(input[i] != '"'){
    i++;
  }
  input = input.substring(2,i);
  char carray[20];
  //Serial.println(input);
  input.toCharArray(carray, sizeof(carray));
  //Serial.println(carray);
  temp = atoi(carray);
  //Serial.println(temp);
  return temp;
}
