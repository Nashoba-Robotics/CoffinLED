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

int INFO_ARM_ANGLE = 0;
int INFO_HOOD = 1;
int INFO_ROLL_INTAKE = 2;
int INFO_ROLL_LOADER = 3;
int INFO_SHOOTER_MOVE = 4;
int INFO_SHOOTER_FULL = 5;
int INFO_BALL_INTAKE = 6;
int INFO_BALL_LOADER = 7;

int dataPin = 4;
int clockPin = 2;
LPD8806 strip = LPD8806(32, dataPin, clockPin);

int cmd = -1;
int data1 = -1;
char delimiter1 = " "; //goes in between pieces of data
char delimiter2 = ":"; //ends of all commands
char delimiter3 = ">"; //begins info commands
char delimiter4 = "["; //begins state commands
int kind = -1; //0 is a state command, 1 is an info command

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
  if(Serial.available() > 0)
  {
    String inputString = Serial.readStringUntil(delimiter2) += delimiter4 + delimiter3;
    //The characters are added because the behavior of indexOf
    //when the string doesn't contain the characters
    //is not given by the docs
    if(inputString.indexOf(delimiter4) > inputString.indexOf(delimiter3)) { //It's a state command
      cmd = inputString.substring(0, inputString.indexOf(delimiter1)).toInt();
      data1 = inputString.substring(inputString.indexOf(delimiter1) + 1, inputString.indexOf(delimiter2)).toInt();
      kind = 0;
      state = cmd;
    } else { //It's an info command
      cmd = inputString.substring(0, inputString.indexOf(delimiter3)).toInt();
      data1 = inputString.substring(inputString.indexOf(delimiter3) + 1, inputString.indexOf(delimiter2)).toInt();
      kind = 1;
    }
    arg1 = data1;
  }


  if(kind == 0) {
    switch(state) {
      case STATE_OFF:
        setStrip(strip.Color(0, 0, 0));
        break;
      case STATE_RAINBOW:
        rainbowCycle(arg1);
        break;
      case STATE_SPLIT_STRIP:
        splitStrip(arg1);
        break;
      case STATE_SCORE_YO:
        scoreStrobe();
        break;
      case STATE_DISCONNECTED:
        disconnectedLight();
        break;
      case STATE_CONNECTED:
        connectedLight();
        break;
    }
  } else if(kind == 1) {
    switch(cmd) {
      case INFO_ARM_ANGLE:
        armAngle(data1);
        break;
      case INFO_HOOD:
        hood(data1);
        break;
      case INFO_ROLL_INTAKE:
        rollIntake(data1);
        break;
      case INFO_ROLL_LOADER:
        rollLoader(data1);
        break;
      case INFO_SHOOTER_MOVE:
        shooterMove(data1);
        break;
      case INFO_SHOOTER_FULL:
        shooterFull(data1);
        break;
      case INFO_BALL_INTAKE:
        ballIntake(data1);
        break;
      case INFO_BALL_LOADER:
        ballLoader(data1);
        break;
    }
  }
}

//TODO: Set all of these
void armAngle(int angle) {}

void hood(int angle) {}

void rollIntake(int on) {}

void rollLoader(int on) {}

void shooterMove(int percent) {}//Percent from 1 to 100

void shooterFull(int on) {}

void ballIntake(int on) {}

void ballLoader(int on) {}

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
  strip.setPixelColor(strip.numPixels()-1, strip.Color(127, 0, 0));
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
  strip.setPixelColor(strip.numPixels()-1, strip.Color(0, 127, 0));
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
