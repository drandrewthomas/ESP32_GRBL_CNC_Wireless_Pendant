// Convert PNG images at http://javl.github.io/image2cpp/
// 250 threshold and invert colors

// Change to suit your LASER:
#define FOCUSGRBL "M3 S5"

#include "Arduino.h"
#include <Wire.h>
#include "cncimages.h";
#include "BluetoothSerial.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
// Adafruit busio library must now be installed in library manager too

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#define ROTARY_A 27
#define ROTARY_B 26
#define ROTARY_SW 25
#define SW_1 19
#define SW_2 18
#define SW_3 5
#define SW_4 4
#define SW_5 2
#define BUZZER 14

BluetoothSerial SerialBT;
int modenum=0,beeplen=0,rotary_alast,rotary_swlast,jogaxis,currfunc=0;
bool sw1=false,sw2=false,sw3=false,firsttime=false,beeping=false;
bool sw1changed=false,sw2changed=false,sw3changed=false,laseron=false;
long beepstart;
Adafruit_SSD1306 display(128,64,&Wire,0);

void setup()
{
  pinMode (ROTARY_A,INPUT);
  pinMode (ROTARY_B,INPUT);
  pinMode (ROTARY_SW,INPUT);
  pinMode (SW_1,INPUT_PULLUP);
  pinMode (SW_2,INPUT_PULLUP);
  pinMode (SW_3,INPUT_PULLUP);
  pinMode (SW_4,INPUT_PULLUP);
  pinMode (SW_5,INPUT);
  pinMode (BUZZER,OUTPUT);
  Serial.begin(115200);
  SerialBT.begin("CNCESP32");
  display.begin(SSD1306_SWITCHCAPVCC,0x3C); // Some displays may be 0x3D
  display.clearDisplay();
  display.drawBitmap(0,0,splash_bmp,splash_width,splash_height,WHITE);
  display.display();
  delay(2000);
  display.clearDisplay();
  drawmode0();
  rotary_alast=digitalRead(ROTARY_A);
  rotary_swlast=digitalRead(ROTARY_SW);
  startbeeping(100);
}

void loop()
{
  if(beeping==true)
  {
    if((millis()-beepstart)>beeplen)
    {
      digitalWrite(BUZZER,LOW);
      beeping=false;
    }
  }
  if(modenum==0)
  {
    if(firsttime==true)
    {
      firsttime=false;
      while(Serial.available()>0) Serial.read();
      while(SerialBT.available()>0) SerialBT.read();
      drawmode0();
    }
    if(Serial.available()>0 || SerialBT.available()>0)
    {
      if(Serial.available()>0)
      {
        SerialBT.write(Serial.read());
      }
      if(SerialBT.available()>0)
      {
        Serial.write(SerialBT.read());
      }
    }
    if(checkbuttons()==2) // OK button is pressed
    {
      modenum=1; // Go to function chooser
      firsttime=true;
    }
  }
  else
  {
    switch(modenum)
    {
      case 1: drawfunctionchooser();
              break;
      case 2: drawjoggingmode();
              break;
      case 3: drawzeroingmode();
              break;
      case 4: drawfocusingmode();
              break;
    }
  }
}

void drawfunctionchooser()
{
  int rotary;
  if(firsttime==true)
  {
    firsttime=false;
    drawfuncscreen();
    delay(50);
    while(checkbuttons()==2); // Check button debounce
    startbeeping(50);
  }
  rotary=rotarycount(200);
  if(rotary>1)
  {
    currfunc++;
    if(currfunc>2) currfunc=0;
    drawfuncscreen();
  }
  if(rotary<-1)
  {
    currfunc--;
    if(currfunc<0) currfunc=2;
    drawfuncscreen();
  }
  if(checkbuttons()==1)
  {
    delay(50);
    while(checkbuttons()==1); // Check button debounce
    modenum=0;
    firsttime=true;
    if(laseron==true)
    {
      Serial.println("M5");
      waitforline();
      laseron=false;
    }
    startbeeping(50);
    return;
  }
  if(checkbuttons()==2)
  {
    delay(50);
    while(checkbuttons()==2); // Check button debounce
    modenum=currfunc+2;
    firsttime=true;
    return;
  }
}

void drawfuncscreen()
{
  display.clearDisplay();
  switch(currfunc)
  {
    case 0: display.drawBitmap(0,0,jogmodefunc_bmp,jogmodefunc_width,jogmodefunc_height,WHITE);
            break;
    case 1: display.drawBitmap(0,0,zeromodefunc_bmp,zeromodefunc_width,zeromodefunc_height,WHITE);
            break;
    case 2: display.drawBitmap(0,0,focusmodefunc_bmp,focusmodefunc_width,focusmodefunc_height,WHITE);
            break;
  }
  display.display();
}

void drawmode0()
{
  display.clearDisplay();
  display.drawBitmap(0,0,mode0bt_bmp,mode0bt_width,mode0bt_height,WHITE);
  display.display();
}

void drawjoggingmode()
{
  int rotary,jogdir;
  float jogdist;
  if(firsttime==true)
  {
    firsttime=false;
    jogaxis=0;
    startbeeping(50);
    drawjoggingdisplay('?');
  }
  if(checkbuttons()==1)
  {
    delay(50);
    while(checkbuttons()==1); // Check button debounce
    modenum=1;
    firsttime=true;
    startbeeping(50);
    return;
  }
  if(checkbuttons()==3) // X button pressed
  {
    delay(50);
    while(checkbuttons()==3); // Check button debounce
    startbeeping(50);
    jogaxis=1;
    drawjoggingdisplay('X');
  }
  if(checkbuttons()==4) // Y button pressed
  {
    delay(50);
    while(checkbuttons()==4); // Check button debounce
    startbeeping(50);
    jogaxis=2;
    drawjoggingdisplay('Y');
  }
  if(checkbuttons()==5) // Z button pressed
  {
    delay(50);
    while(checkbuttons()==5); // Check button debounce
    startbeeping(50);
    jogaxis=3;
    drawjoggingdisplay('Z');
  }
  if(jogaxis>0)
  {
    if(jogaxis==3) jogdist=0.125;
    else jogdist=0.25;
    jogdir=0;
    if(digitalRead(ROTARY_SW)==LOW)jogdist*=3;
    rotary=rotarycount(300);
    if(rotary!=0)
    {
      if(rotary<0)
      {
        jogdir=1;
        rotary*=-1;
      }
      if(rotary>5) rotary=5;
      jogdist*=float(rotary);
      Serial.print("G91 G0 ");
      switch(jogaxis)
      {
        case 1: Serial.print('X'); break;
        case 2: Serial.print('Y'); break;
        case 3: Serial.print('Z'); break;
      }
      if(jogdir==1) Serial.print('-');
      Serial.print(jogdist,DEC);
      Serial.println(" F200");
      waitforline();
    }
  }
}

void drawjoggingdisplay(char c)
{
  display.clearDisplay();
  display.drawBitmap(0,8,jogging_bmp,jogging_width,jogging_height,WHITE);
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(70,5);
  display.println("Axis");
  display.setTextSize(4);
  display.setCursor(85,33);
  display.println(c);
  display.display();
}

void drawzeroingmode()
{
  if(firsttime==true)
  {
    firsttime=false;
    startbeeping(50);
    drawzeroingdisplay('?');
  }
  if(checkbuttons()==1)
  {
    delay(50);
    while(checkbuttons()==1); // Check button debounce
    modenum=1;
    firsttime=true;
    startbeeping(50);
    return;
  }
  if(checkbuttons()==3) // X button pressed
  {
    delay(50);
    while(checkbuttons()==3); // Check button debounce
    startbeeping(50);
    Serial.println("G10 L20 P1 X0");
    waitforline();
    startbeeping(100);
    drawzeroingdisplay('X');
  }
  if(checkbuttons()==4) // Y button pressed
  {
    delay(50);
    while(checkbuttons()==4); // Check button debounce
    startbeeping(50);
    Serial.println("G10 L20 P1 Y0");
    waitforline();
    startbeeping(100);
    drawzeroingdisplay('Y');
  }
  if(checkbuttons()==5) // Z button pressed
  {
    delay(50);
    while(checkbuttons()==5); // Check button debounce
    startbeeping(50);
    Serial.println("G10 L20 P1 Z0");
    waitforline();
    startbeeping(100);
    drawzeroingdisplay('Z');
  }
}

void drawzeroingdisplay(char c)
{
  display.clearDisplay();
  display.drawBitmap(0,8,zeroing_bmp,zeroing_width,zeroing_height,WHITE);
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(70,5);
  display.println("Axis");
  display.setTextSize(4);
  display.setCursor(85,33);
  display.println(c);
  display.display();
}

void drawfocusingmode()
{
  if(firsttime==true)
  {
    firsttime=false;
    if(laseron==false)
    {
      laseron=true;
      startbeeping(100);
      display.clearDisplay();
      display.drawBitmap(0,0,focusing_bmp,focusing_width,focusing_height,WHITE);
      display.display();
      Serial.println(FOCUSGRBL);
      waitforline();
    }
    else
    {
      laseron=false;
      startbeeping(50);
      display.clearDisplay();
      display.drawBitmap(0,0,focusingoff_bmp,focusingoff_width,focusingoff_height,WHITE);
      display.display();
      Serial.println("M5");
      waitforline();
    }
  }
  if(checkbuttons()==1)
  {
    delay(50);
    while(checkbuttons()==1); // Check button debounce
    modenum=1;
    firsttime=true;
    startbeeping(50);
    return;
  }
}

void waitforline()
{
  char c=0;
  while(c!=10)
  {
    if(Serial.available()>0)
    {
      c=Serial.read();
    }
    if(checkbuttons()==1)
    {
      delay(50);
      while(checkbuttons()==1); // Check button debounce
      return;
    }
  }
}

int checkbuttons()
{
  if(digitalRead(SW_1)==LOW) return 1;
  if(digitalRead(SW_2)==LOW) return 2;
  if(digitalRead(SW_3)==LOW) return 3;
  if(digitalRead(SW_4)==LOW) return 4;
  if(digitalRead(SW_5)==LOW) return 5;
  if(digitalRead(ROTARY_SW)==LOW) return 6;
  return 0;
}

int rotarycount(int t)
{
  int count=0,a;
  long stime=millis();
  while((millis()-stime)<t)
  {
    a=digitalRead(ROTARY_A);
    if(a!=rotary_alast)
    {
      if(digitalRead(ROTARY_B)!=a) count++;
      else count--;
    }
    rotary_alast=a;
  }
  return count;
}

void startbeeping(int bl)
{
  beeplen=bl;
  beepstart=millis();
  beeping=true;
  digitalWrite(BUZZER,HIGH);
}
