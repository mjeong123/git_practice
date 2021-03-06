#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif
 #define PIN            6
 #define NUMPIXELS      110
 Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_RGBW + NEO_KHZ800);
 int delayval = 50; // delay for half a second
 void setup() {
   Serial.begin(115200);    
  pinMode(3, INPUT);
  pinMode(2, INPUT);
#if defined (__AVR_ATtiny85__)
  if (F_CPU == 16000000) clock_prescale_set(clock_div_1);
#endif
  pixels.begin(); // This initializes the NeoPixel library.
 Serial.println("01");
}
 void loop() {
 if((digitalRead(2)== LOW )&&(digitalRead(3)==LOW))
{
  for(int i=0;i<26;i++){
   pixels.setPixelColor(i, pixels.Color(0,0,0,150)); 
   Serial.println("1");
   }
  pixels.show();
  delay(delayval);
  
}
 else if((digitalRead(2)==LOW)&&(digitalRead(3)==HIGH))
{
  for(int i=26;i<50;i++){
   pixels.setPixelColor(i, pixels.Color(0,0,150,0)); 
Serial.println("2");
   }
  pixels.show();
  delay(delayval);
   
}
else if((digitalRead(2)==HIGH)&&(digitalRead(3)==LOW))
{
  for(int i=50;i<76;i++){
   pixels.setPixelColor(i, pixels.Color(50,50,0,0)); 
  Serial.println("3");
   }
  pixels.show();
  delay(delayval);
    
}
else if((digitalRead(2)==HIGH)&&(digitalRead(3)==HIGH))
{ for(int i=77;i<105;i++){
    pixels.setPixelColor(i, pixels.Color(20,150,0,0)); 
   Serial.println("4");
   }
  pixels.show();
  delay(delayval);
   
}
 }
