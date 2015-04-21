#include <SPI.h>
#include <LPD8806.h>


int dataPin = 4;
int clockPin = 2;
LPD8806 strip = LPD8806(32, dataPin, clockPin);

int cmd = -1;
int data1 = -1;
String delimiter1 = " ";
String delimiter2 = ":";

void setup()
{
  // put your setup code here, to run once:
  strip.begin();

  Serial.begin(9600);

  // Update the strip, to start they are all 'off'
  strip.show();
}

void loop() 
{
  /* put your main code here, to run repeatedly:
  if(Serial.available() > 0)
  {
    String s = Serial.readStringUntil('\n');
    cmd = inputString.substring(0, inputString.indexOf(delimiter1)).toInt();
    data1 = inputString.substring(inputString.indexOf(delimiter1) + 1, inputString.indexOf(delimiter2)).toInt();
    
  }*/
  rainbowCycle(0);
}

void rainbowCycle(uint8_t wait) {
  uint16_t i, j;
  
  for (j=0; j < 384 * 5; j++) {     // 5 cycles of all 384 colors in the wheel
    for (i=0; i < strip.numPixels(); i++) {
      // tricky math! we use each pixel as a fraction of the full 384-color wheel
      // (thats the i / strip.numPixels() part)
      // Then add in j which makes the colors go around per pixel
      // the % 384 is to make the wheel cycle around
      strip.setPixelColor(i, Wheel( ((i * 384 / strip.numPixels()) + j) % 384) );
    }  
    strip.show();   // write all the pixels out
    delay(wait);
  }
}

uint32_t Wheel(uint16_t WheelPos)
{
  byte r, g, b;
  switch(WheelPos / 128)
  {
    case 0:
      r = 127 - WheelPos % 128;   //Red down
      g = WheelPos % 128;      // Green up
      b = 0;                  //blue off
      break; 
    case 1:
      g = 127 - WheelPos % 128;  //green down
      b = WheelPos % 128;      //blue up
      r = 0;                  //red off
      break; 
    case 2:
      b = 127 - WheelPos % 128;  //blue down 
      r = WheelPos % 128;      //red up
      g = 0;                  //green off
      break; 
  }
  return(strip.Color(r,g,b));
}
