/* READ ME!!! ---> THIS IS VERY IMPORTANT
 you must click "Sketch" then "Import Library" then "add library"
 in the window that opened, you must navigate to the "lib" directory
 in this project, then add the "PololuLedStrip" library.
 */
#include <PololuLedStrip.h>

#define LED_COUNT 60
#define CMD_OFF 0
#define CMD_SET 1
#define CMD_PTRN 2

#define PTRN_RAINBOW 0
#define PTRN_GRADIENT 1
#define PTRN_LRBOUNCE 2
#define PTRN_INOUTBOUNCE 3
#define PTRN_LOADING 4
#define PTRN_BOUNCE

PololuLedStrip<6> ledStrip;//<6> on UNO, Leonardo, and Duemilanove, <3> on Mega
rgb_color colors[LED_COUNT];
rgb_color blank[LED_COUNT];

//Communication stuff:
String input = "";
int pbCmd = -1;
int pbData1 = -1;
int pbData2 = -1;
int pbData3 = -1;
String delimiter1 = " ";
String delimiter2 = ":";
String delimiter3 = ";";
String delimiter4 = "+";

String inputString;

//State stuff:
String state = "rainbow";
rgb_color color = rgbColor(255,255,255);

//Moving pattern stuff:
int moveSpeed = 5;

//Set stuff:
int startPosition = 0;
int endPosition = 60;

//In Out or Left Right Bounce stuff:
int bouncePosition = 30;
int length = 10;
boolean bounceDirection = true;

//Gradient stuff:
int grad0[3] = {//start color
  255, 0, 0
};
int grad50[3] = {//middle color
  0, 255, 0
};
int grad100[3] = {//end color
  0, 0, 255
};
float gradChange1[3] = {
  (float) (grad50[0] - grad0[0]) / LED_COUNT, 
  (float) (grad50[1] - grad0[1]) / LED_COUNT, 
  (float) (grad50[2] - grad0[2]) / LED_COUNT
};
float gradChange2[3] = {
  (float) (grad100[0] - grad50[0]) / LED_COUNT, 
  (float) (grad100[1] - grad50[1]) / LED_COUNT, 
  (float) (grad100[2] - grad50[2]) / LED_COUNT
};

//Loading Bar stuff:
int progress = 0;

//Bounce stuff:
int bouncePositions[3] = {
  5, 25, 50};
//False = closer, true = further
boolean bounceDirections[3] = {
  true, false, false};
boolean bounceChanged[3] = {
  false, false, false};

rgb_color bounceColors[3] = {
  (rgb_color){
    255,0,0              }
  ,(rgb_color){
    0,0,255              }
  ,
  (rgb_color){
    0,255,0  
  }
};

void setup() {
  for(uint16_t i = 0; i < LED_COUNT; i++) {
    blank[i] = rgbColor(0, 0, 0);
  }
  clearLED();
  Serial.begin(9600);

  Serial.println("Listening for commands");
  //wdt_enable(WDTO_1S);
}

void loop() 
{
  //wdt_reset();
  for(uint16_t i = 0; i < LED_COUNT; i++) {
    colors[i] = rgbColor(0, 0, 0);
  }
  
  if (Serial.available() > 0) 
  {
    String inputString = Serial.readStringUntil('\n');
    pbCmd = inputString.substring(0, inputString.indexOf(delimiter1)).toInt();
    pbData1 = inputString.substring(inputString.indexOf(delimiter1) + 1, inputString.indexOf(delimiter2)).toInt();
    pbData2 = inputString.substring(inputString.indexOf(delimiter2) + 1, inputString.indexOf(delimiter3)).toInt();
    pbData3 = inputString.substring(inputString.indexOf(delimiter3) + 1, inputString.indexOf(delimiter4)).toInt();
    delay(1);
    Serial.print(pbCmd);
    Serial.print(" ");
    Serial.print(pbData1);
    Serial.print(":");
    Serial.print(pbData2);
    Serial.print(";");
    Serial.println(pbData3);
    Serial.println(inputString);
    procCmd(pbCmd, pbData1, pbData2, pbData3);
  }
  if(state == "off")
  {
    setLED(0, LED_COUNT, rgbColor(0, 0, 0));
  }  
  else{
    if(state == "set")
    {
      setLED(startPosition, endPosition, color);
    }  
    else{
      if(state == "rainbow")
      {
        rainbow();
      }
      else{
        if(state == "gradient")
        {
          gradient();
        }
        else{
          if(state == "lrbounce")
          {
            leftRightBounce();
          }
          else{
            if(state == "inoutbounce")
            {
              inOutBounce();
            }
            else{
              if(state == "loading")
              {
                loading();
              }
              else{
                if(state == "bounce")
                {
                  bounce();
                }
              }
            }
          }
        }
      }
    }
  } 
  updateLED();
}

void procCmd(int cmd, int data1, int data2, int data3) {
  switch(cmd) {
  case CMD_OFF:
    state = "off";
  case CMD_SET:
    startPosition = data1;
    endPosition = data2;
    state = "set";
    break;
  case CMD_PTRN:
    if(data1 == PTRN_RAINBOW)
    {
      state = "rainbow";
      moveSpeed = data2;
    }
    else{
      if(data1 == PTRN_GRADIENT)
      {
        state = "gradient";
        startPosition = data2;
        endPosition = data3;
      }
      else{
        if(data1 == PTRN_LRBOUNCE)
        {
          state = "lrbounce";
        }
        else{
          if(data1 == PTRN_INOUTBOUNCE)
          {
            state = "inoutbounce";
          }
          else{
            if(data1 == PTRN_LOADING)
            {
              state = "loading";
              progress = data2;
            }
          }
        }
      }
    }
    break;
  }
}

//States:

void setLED(int startPosition, int endPosition, rgb_color rgbcolor) {
  for(uint16_t i = startPosition; i < endPosition; i++) {
    colors[i] = rgbcolor;
  }
}

void rainbow()
{
  uint16_t time = millis()*moveSpeed >> 2;
  for(uint16_t i = 0; i < LED_COUNT; i++)
  {
    byte x = (time >> 2) - (i << 3);
    colors[i] = hsvToRgb((uint32_t)x * 359 / 256, 255, 255);
  }
}

void gradient()
{
  for(uint16_t i = startPosition; i < endPosition - startPosition; i++) {
    if(i < LED_COUNT) {
      if(i < endPosition / 2) {
        colors[i] = rgbColor((int) (grad0[0] + i * gradChange1[0] * 2), 
        (int) (grad0[1] + i * gradChange1[1] * 2), 
        (int) (grad0[2] + i * gradChange1[2] * 2));
      } 
      else if(i >= endPosition / 2) {
        colors[i] = rgbColor((int) (grad50[0] + (i - endPosition / 2) * gradChange2[0] * 2), 
        (int) (grad50[1] + (i - endPosition / 2) * gradChange2[1] * 2), 
        (int) (grad50[2] + (i - endPosition / 2) * gradChange2[2] * 2));
      }
    } 
  }
}

void leftRightBounce()
{
  for(uint16_t i = bouncePosition; i < (length+bouncePosition); i++) {
    colors[i] = rgbColor(255,255,255);
  }
  if(bouncePosition + length == LED_COUNT)
  {
    bounceDirection = !bounceDirection;
  }
  if(bouncePosition == 0)
  {
    bounceDirection = !bounceDirection;
  }
  if(bounceDirection)
  {
    bouncePosition++;
  }
  else  {
    bouncePosition--;
  }
  delay(20);
}

void inOutBounce() {
  for(uint16_t i = bouncePosition; i < (length+bouncePosition); i++) {
    colors[i] = rgbColor(255,255,255);
    if(i >= 60){
      colors[i-LED_COUNT] = rgbColor(255,255,255);
    }
  }

  if(bouncePosition > LED_COUNT)
  {
    bouncePosition = bouncePosition - LED_COUNT;
  }
  bouncePosition++;
  delay(20);
}

void loading() {
  setLED(0, progress, rgbColor(255,255,255));
  progress++;
  if(progress > 60)
  {
    progress = 0;
  }
  delay(20);
}

void bounce(){
  for(int i = 0; i < sizeof(bouncePositions)/sizeof(int); i++)
  {
    colors[bouncePositions[i]] = bounceColors[i];
  }
  for(int i = 0; i < sizeof(bouncePositions)/sizeof(int); i++)
  {
    if((bouncePositions[i]) > LED_COUNT)
    {
      bouncePositions[i] = 1;
    }
    if(bouncePositions[i] < 1)
    {
      bouncePositions[i] = LED_COUNT;
    }

    if(bounceDirections[i])
    {
      bouncePositions[i]++;
    }
    else{
      bouncePositions[i]--;
    }

    for(int j = 0; j < sizeof(bouncePositions)/sizeof(int); j++)
    {
      if(i != j){
        if(bounceChanged[j] == false)
        {
          if(bouncePositions[i] == bouncePositions[j] || (bouncePositions[i] == LED_COUNT && bouncePositions[j] == 1) || (bouncePositions[i] == 1 && bouncePositions[j] == LED_COUNT))
          {
            bounceDirections[i] = !bounceDirections[i];
            bounceChanged[i] = true;
            bounceChanged[j] = true;
          }
        }
      }
    }

    if(bounceChanged[i] == true)
    {

      if(bounceDirections[i])
      {
        bouncePositions[i]++;
      }
      else{
        bouncePositions[i]--;
      }
      bounceChanged[i] = false;
      if((bouncePositions[i] - 1) >= LED_COUNT)
      {
        bouncePositions[i] = 0;
      }
      if(bouncePositions[i] < 0)
      {
        bouncePositions[i] = LED_COUNT;
      }
    }
  }
  delay(20);
}

//LED Management

void updateLED() {
  ledStrip.write(colors, LED_COUNT);
}

void clearLED() {
  ledStrip.write(blank, LED_COUNT);
}

//Color stuff:
rgb_color hsvToRgb(uint16_t h, uint8_t s, uint8_t v) {
  uint8_t f = (h % 60) * 255 / 60;
  uint8_t p = v * (255 - s) / 255;
  uint8_t q = v * (255 - f * s / 255) / 255;
  uint8_t t = v * (255 - (255 - f) * s / 255) / 255;
  uint8_t r = 0, g = 0, b = 0;
  switch((h / 60) % 6) {
  case 0: 
    r = v; 
    g = t; 
    b = p; 
    break;
  case 1: 
    r = q; 
    g = v; 
    b = p; 
    break;
  case 2: 
    r = p; 
    g = v; 
    b = t; 
    break;
  case 3: 
    r = p; 
    g = q; 
    b = v; 
    break;
  case 4: 
    r = t; 
    g = p; 
    b = v; 
    break;
  case 5: 
    r = v; 
    g = p; 
    b = q; 
    break;
  }
  return (rgb_color) { 
    r, g, b};
}

rgb_color rgbColor(uint16_t r, uint16_t g, uint16_t b) {
  return (rgb_color) {
    r, g, b};
}
