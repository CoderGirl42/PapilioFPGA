/*
 Gadget Factory
 LogicStart MegaWing Example

 Tutorials:

 Related library documentation:
  http://papilio.cc/index.php?n=Papilio.VGA

 Hardware:
   This example circuit is wired for the LogicStart MegaWing

 created 2014
 by Alvaro Lopes and Jack Gassett
 http://www.gadgetfactory.net
 
 This example code is in the public domain.
 */
#define circuit LogicStart_MegaWing

#define FREQ 17000          //Freq for all players 

#define LS_JOY_RIGHT WB11
#define LS_JOY_LEFT  WB12
#define LS_JOY_DOWN  WB13
#define LS_JOY_UP    WB14
#define LS_JOY_SELECT WB15

#include "HQVGA.h"
#include <SevenSegHW.h>
#include "SPIADC.h"
#include "SPI.h"
#include <Timer.h>

//Comment out this section for Papilio One.
/*#ifdef __ZPUINO_PAPILIO_PRO__
  #include <SD.h>
  #include "SmallFS.h"
  #include "modplayer.h"
  #include "ramFS.h"
  #include "cbuffer.h"
  MODPLAYER modplayer;
#endif*/

SEVENSEGHW sevenseg;

int ledPins[] = { 
  WC8, WC9, WC10, WC11, WC12, WC13, WC14, WC15  };       // an array of pin numbers to which LEDs are attached
int ledCount = 8;           // the number of pins (i.e. the length of the array)

int switchPins[] = { 
  WC0, WC1, WC2, WC3, WC4, WC5, WC6, WC7 };       // an array of pin numbers to which Buttons are attached
int switchCount = 8;           // the number of pins (i.e. the length of the array)

int buttonState = 0;         // variable for reading the pushbutton status
int thisPin;
int ledState = LOW;
unsigned adcvalue = 5;
int cnt = 0;
int extcnt = 0;
int mode = 0;
unsigned channel=0;
int timeout=0;
int shift=0;
char byteValue = 0;
bool joyLeft = false;
bool joyRight = false;
bool joyCenter = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
#ifdef __ZPUINO_PAPILIO_PRO__
   //Start SmallFS
  if (SmallFS.begin()<0) {
	Serial.println("No SmalLFS found.");
  }
  else{
     Serial.println("SmallFS Started.");
  }  
  
  //Set what wishbone slot the audio passthrough device is connected to.
  modplayer.setup(5);

  modplayer.loadFile("music.mod");
  modplayer.play(true); 
 
 //Setup timer for YM and mod players, this generates an interrupt at 1700hz
  Timers.begin();
    int r = Timers.periodicHz(17000, (bool(*)(void*))timer, 0, 1);
    if (r<0) {
        Serial.println("Fatal error!");
    }   
  
#endif  
  

  
  //Setup VGA Hello World
  VGA.begin(VGAWISHBONESLOT(9),CHARMAPWISHBONESLOT(10));
  VGA.clear();
  VGA.setBackgroundColor(BLACK);
  VGA.setColor(RED);
  VGA.printtext(35,10,"Hello World");
  VGA.printtext(15,30, "Hello World RED");
  VGA.setColor(GREEN);
  VGA.printtext(15,40, "Hello World GREEN");
  VGA.setColor(BLUE);
  VGA.printtext(15,50, "Hello World BLUE");
  VGA.setColor(YELLOW);
  VGA.printtext(15,60, "Hello World YELLOW");
  VGA.setColor(PURPLE);
  VGA.printtext(15,70, "Hello World PURPLE");
  VGA.setColor(CYAN);
  VGA.printtext(15,80, "Hello World CYAN");
  VGA.setColor(WHITE);
  VGA.printtext(15,90, "Hello World WHITE");   
  VGA.setBackgroundColor(WHITE);  
  VGA.setColor(BLACK);  
  VGA.printtext(15,100, "Hello World BLACK"); 
  
  sevenseg.begin(11);
  sevenseg.setBrightness(15);
  sevenseg.setHexValue(0000);
  
  // initialize the LED pins as an output:
  for (int thisPin = 0; thisPin < ledCount; thisPin++)  {
    pinMode(ledPins[thisPin], OUTPUT);  
  }
  
  // initialize the switch pins as an input:
  for (int thisPin = 0; thisPin < ledCount; thisPin++)  {
    pinMode(switchPins[thisPin], INPUT);      
  }    
  
  //Start SPI ADC for analog input
  analog.begin(CS(12),WISHBONESLOT(6),ADCBITS(SPIADC_12BIT));
  
}

#ifdef __ZPUINO_PAPILIO_PRO__
bool timer(void)
{
  //Interrupt runs at 17KHz
  modplayer.zpu_interrupt();
  return true;
}
#endif 


void chanUpdate()
{
        timeout = 1700;
        #ifdef __ZPUINO_PAPILIO_ONE__
          timeout = 300;
        #endif     
	sevenseg.setDigitValue(channel,0,3);
        sevenseg.custom(0,2);
        sevenseg.custom(SEGF|SEGE|SEGB|SEGC|SEGG,1);
        sevenseg.setHexValue(0xC,0);
}

void loop() {
  // put your main code here, to run repeatedly: 
#ifdef __ZPUINO_PAPILIO_PRO__
  if (modplayer.getPlaying() == 1)
    modplayer.audiofill();
#endif     
    
  //Handle LED's and Switches
  for (int thisPin = 0; thisPin < switchCount; thisPin++)  {
    // read the state of the pushbutton value:
    buttonState = digitalRead(switchPins[thisPin]);
  
    // check if the switch is on.
    // if it is, the buttonState is HIGH:
    if (buttonState == HIGH) {     
       // turn LED on:    
       digitalWrite(ledPins[thisPin], HIGH); 
       switch(thisPin)
       {
          case 0:
            byteValue |= (1 << 0x00);
            break;
          case 1:
            byteValue |= (1 << 0x01);
            break;
          case 2:
            byteValue |= (1 << 0x02);
            break;
          case 3:
            byteValue |= (1 <<  0x03);
            break;
          case 4:
            byteValue |= (1 <<  0x04);
            break;
          case 5:
            byteValue |= (1 <<  0x05);
            break;
          case 6:
            byteValue |= (1 <<  0x06);
            break;
          case 7:
            byteValue |= (1 <<  0x07);
            break;
       } 
    } 
    else {
      // toggle LED:
      digitalWrite(ledPins[thisPin], LOW); 
      switch(thisPin)
       {
          case 0:
            byteValue &= ~(1 << 0x00);
            break;
          case 1:
            byteValue &= ~(1 << 0x01);
            break;
          case 2:
            byteValue &= ~(1 << 0x02);
            break;
          case 3:
            byteValue &= ~(1 << 0x03);
            break;
          case 4:
            byteValue &= ~(1 << 0x04);
            break;
          case 5:
            byteValue &= ~(1 << 0x05);
            break;
          case 6:
            byteValue &= ~(1 << 0x06);
            break;
          case 7:
            byteValue &= ~(1 << 0x07);
            break;
       } 
    }
  }    
  
  //Handle joystick and reading SPI ADC
  if ((extcnt & 0x17) == 0) {
    /* Check for left joy stick press */
    if (!digitalRead(LS_JOY_LEFT) && !joyLeft) {
      joyLeft = true;
    }
    
    /* Check for left joy stick release */
    if (digitalRead(LS_JOY_LEFT) && joyLeft) {
      joyLeft = false;
      shift++;
    }
    
    /* Check for right joy stick press */
    if (!digitalRead(LS_JOY_RIGHT) && !joyRight) {
      joyRight = true;
    }
    
    /* Check for right joy stick release */
    if (digitalRead(LS_JOY_RIGHT) && joyRight) {
      joyRight = false;
      shift--;
    }
    
    /* Check for right joy stick press */
    if (!digitalRead(LS_JOY_SELECT) && !joyCenter) {
      joyCenter = true;
    }
    
    /* Check for right joy stick release */
    if (digitalRead(LS_JOY_SELECT) && joyCenter) {
      joyCenter = false;
      shift = 0;
    }
    
    /* Show Output. */
    if(shift >= 0) {
       sevenseg.setIntValue(byteValue << shift, 0);
    } else {
      sevenseg.setIntValue(byteValue >> (shift * -1), 0);
    }
  }
      extcnt++;
      if (timeout!=0)
        timeout--;
      #ifdef __ZPUINO_PAPILIO_ONE__
        delay(1);
      #endif          
}
