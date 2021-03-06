#include <SPI.h>
#include <LPD8806.h>


int state = 0;
int arg1 = 0;

int STATE_OFF = 0;
int STATE_RAINBOW = 1;
int STATE_SPLIT_STRIP = 2;
int STATE_SCORE_YO = 3;
int STATE_DISCONNECTED = 4;
int STATE_CONNECTED = 5;
int STATE_BOUNCE = 6;

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
  Serial.println("Waiting for Commands");

  // Update the strip, to start they are all 'off'
  strip.show();
}

void loop() 
{
  //put your main code here, to run repeatedly:
  if(Serial.available() > 0)
  {
    String inputString = Serial.readStringUntil('\n');
    cmd = inputString.substring(0, inputString.indexOf(delimiter1)).toInt();
    data1 = inputString.substring(inputString.indexOf(delimiter1) + 1, inputString.indexOf(delimiter2)).toInt();
    state = cmd;
    arg1 = data1;
  }
  
  if(state == STATE_OFF)
  {
    setStrip(strip.Color(0, 0, 0));
  }
  else if(state == STATE_RAINBOW)
  {
    rainbowCycle(arg1);
  }
  else if(state == STATE_SPLIT_STRIP)
  {
   splitStrip(arg1); 
  }
  else if(state == STATE_SCORE_YO)
  {
   scoreStrobe(); 
  }
  else if(state == STATE_DISCONNECTED)
  {
   disconnectedLight(); 
  }
  else if(state == STATE_CONNECTED)
  {
    connectedLight();
  }
  else if(state == STATE_BOUNCE)
  {
   bounce(); 
  }
}

uint16_t rainbowState = 0;
void rainbowCycle(uint8_t wait)
{
  uint16_t i;
  for (i=0; i < strip.numPixels(); i++) {
    // tricky math! we use each pixel as a fraction of the full 384-color wheel
    // (thats the i / strip.numPixels() part)
    // Then add in j which makes the colors go around per pixel
    // the % 384 is to make the wheel cycle around
    strip.setPixelColor(i, Wheel( ((i * 384 / strip.numPixels()) + rainbowState) % 384) );
  } 
  strip.show();   // write all the pixels out
  delay(wait+1);
  rainbowState = (rainbowState+1)%(384*5);
}

void setStrip(uint32_t color)
{
  uint16_t i;
  for(i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, color);
  }
  strip.show();
}

void disconnectedLight()
{
  uint16_t i;
  for(i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, strip.Color(0, 0, 0)); 
  }
  strip.setPixelColor(0, strip.Color(127, 0, 0));
  strip.setPixelColor(1, strip.Color(127, 0, 0));
  strip.setPixelColor(2, strip.Color(127, 0, 0));
  strip.setPixelColor(strip.numPixels()-1, strip.Color(127, 0, 0));
  strip.setPixelColor(strip.numPixels()-2, strip.Color(127, 0, 0));
  strip.setPixelColor(strip.numPixels()-3, strip.Color(127, 0, 0));
  strip.show();
}

void connectedLight()
{
  uint16_t i;
  for(i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, strip.Color(0, 0, 0)); 
  }
  strip.setPixelColor(0, strip.Color(0, 127, 0));
  strip.setPixelColor(1, strip.Color(0, 127, 0));
  strip.setPixelColor(2, strip.Color(0, 127, 0));
  strip.setPixelColor(strip.numPixels()-1, strip.Color(0, 127, 0));
  strip.setPixelColor(strip.numPixels()-2, strip.Color(0, 127, 0));
  strip.setPixelColor(strip.numPixels()-3, strip.Color(0, 127, 0));
  strip.show();
}

void splitStrip(int amount)
{
  int i;
  uint32_t color;
  if(amount <= 5)
  {
   color = strip.Color(127, 0, 0); 
  }
  else if(amount <= 10)
  {
   color = strip.Color(127, 127, 0); 
  }
  else
  {
   color = strip.Color(127, 127, 127); 
  }
  
  for(i = 0; i < amount; i++)
  {
    strip.setPixelColor(i, color);
  }
  for(i = amount; i < strip.numPixels()-amount; i++)
  {
   strip.setPixelColor(i, strip.Color(0, 0, 0));
  } 
  for(i = strip.numPixels(); i > strip.numPixels()-amount-1; i--)
  {
   strip.setPixelColor(i, color); 
  }
  strip.show();
}

int bounceState = 0;
int bounceDirection = 1;
int bounceCount = 0;
uint32_t bounceColors[] = {strip.Color(127, 127, 127), strip.Color(127, 0, 0),strip.Color(0, 127, 0), strip.Color(0, 0, 127)};
int colorIndex = 0;
void bounce()
{
  uint16_t i;
  for(i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, strip.Color(0, 0, 0)); 
  }
  
  bounceCount++;
  if(bounceCount > 30)
  {
    if(bounceState + bounceDirection >= strip.numPixels()+10
    || bounceState + bounceDirection < 0)
    {
      if(bounceState = 1)
      {
        colorIndex = (colorIndex+1)% (4);
      }
      bounceState = 0;
    }
  
    bounceState += bounceDirection;
    bounceCount = 0;
  }
  if(bounceState < 16)
  {
    strip.setPixelColor(bounceState, bounceColors[colorIndex]);
  }
  if(bounceState > 26)
  {
    strip.setPixelColor(bounceState-10, bounceColors[colorIndex]);
  }
  strip.show();
}

int scoreStrobeCount = 0;
int scoreState = 1;
void scoreStrobe()
{
  int i;
  if(scoreState == 1)
  {
    for(i = 0; i < strip.numPixels(); i++)
    {
     strip.setPixelColor(i, strip.Color(0, 127, 0)); 
    }
    strip.show();
  }
  else
  {
    for(i = 0; i < strip.numPixels(); i++)
    {
     strip.setPixelColor(i, strip.Color(0, 0, 0)); 
    }
    strip.show();
  }
  
  scoreStrobeCount++;
  if(scoreStrobeCount > 50)
  {
     scoreState = -scoreState;
     scoreStrobeCount = 0;
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
