/*
  Version 0.6  -  
 
 This uses the original ShiftBrite demo code to write the array.
 It's using the default ShiftBrite Shield pinouts, and you do
 need the extra power if you are using more than three ShiftBrites.
 (Otherwise, you may overload the regulator on the Arduino board!) */
 
//Define constants which should NEVER change (maybe use "const" instead?)
#define clockpin 13 // CI
#define enablepin 10 // EI
#define latchpin 9 // LI
#define datapin 11 // DI  
#define NumLEDs 6   // how many Shiftbrites are defined in each section below? 

// This is using the initial code from the LCD Library page at arduino.cc
// include the library code:
#include <LiquidCrystal.h>
 
/* LCD RS pin to digital pin 6  (because the Shiftbrite uses SPI, which fouls pin 12...)
 * LCD Enable pin to digital pin 7  (avoiding a conflict with Shiftbrite on #11)
 * LCD D4 pin to digital pin 5
 * LCD D5 pin to digital pin 4
 * LCD D6 pin to digital pin 3
 * LCD D7 pin to digital pin 2
 */

/* initialize the library with the numbers of the interface pins
   The format is;  lcd(RS, EN, D4, D5, D6, D7)  */
LiquidCrystal lcd(6, 7, 5, 4, 3, 2);

int analogSwitchPin = A0;  // Switches will tap a series of resistors
int analogRedPin = A1;  // Analog input pin, red potentiometer input
int analogGreenPin = A2;  // Analog input pin, green potentiometer input
int analogBluePin = A3;  // Analog input pin, blue potentiometer input

//Define 'Globals' (variables available to all functions)
// first, for the LED array...
int LEDChannels[NumLEDs][3] = {0};
int SB_CommandMode;
int SB_RedCommand;
int SB_GreenCommand;
int SB_BlueCommand; 

// next, for the variables we want to pass around...

/* Prepare the colors array
 myBase is modulo 15, with R-G-B values, from brightest to dimmest
 0=white, 15=red, 30=yellow, 45=green, 60=blue, 75=teal, 90=purple
 value 135=black (all zeroes).
 myColorCode is modulo 3, defining one shade of a given color in R-G-B.
 You can use any modulo-3 number in the array, from 0 (white) to 135 (black).
 18=almost-brightest red, 51 is medium-green, 72 is dimmest-blue. */
int myColors[] = {
  //(0) grays
  1023, 1023, 1023,
  800, 800, 800,
  600, 600, 600,
  400, 400, 400,
  200, 200, 200,
  //(1,15) salmon, brite orange
  1023, 310, 0,
  875, 190, 0,
  580, 100, 0,
  300, 50, 0,
  60, 10, 0,
  //(2,30) brite green-yellow
  650, 1023, 0,
  560, 850, 0,
  440, 610, 0,
  235, 355, 0,
  60, 75, 0,
  //(3,45) sea green
  0, 1023, 95,
  0, 850, 80,
  0, 600, 65,
  0,330, 35,
  0, 75, 15,
  //(4,60) brite yellow
  1023, 1023, 0,
  880, 880, 0,
  575, 575, 0,
  280, 280, 0,
  75, 75, 0,
  //(5,75) sky blue
  0, 1023, 500,
  0, 890, 390,
  0, 650, 235,
  0, 375, 125,
  0, 70, 30,
  //(6,90) teal
  0, 1023, 125,
  0, 875, 100,
  0, 670, 75,
  0, 325, 45,
  0, 85, 10,
  //(7,105) violet 1
  330, 0, 1023,
  240, 0, 850,
  180, 0, 630,
  100, 0, 370,
  30, 0, 95,
  //(8,120) violet 2
  1023, 0, 700,
  850, 0, 590,
  530, 0, 350,
  260, 0, 130,
  75, 0, 30,
  //(9,135) yellow 2
  1023, 700, 0,
  790, 565, 0,
  450, 270, 0,
  220, 140, 0,
  50, 40, 0,
  //(10,150) fuschia
  1023, 0, 185,
  880, 0, 170,
  540, 0, 105,
  180, 0, 40,
  40, 0, 10,
  //(11,165) black
  0};
  
  int colorMax = 11;  //  How many color groups do we have?
  int colorSet = 0;
  int redValue = 0;
  int greenValue = 0;
  int blueValue = 0;
  
  int arrayGroup = 3;  // zero = "Tuning", other numbers = Color Group #
  int arraySet = 3;  // Active set #, 1-5 are valid.
 
  int mode = 0;  // zero = reading from the color set, one = tuning by pots
  int button = 0;  // which button, if any, is pushed?  Numbered bottom to top
    // button 1 = Group-, button 2 = Group+, button 3 = Set-, button 4 = Set+
    // button 5 = shift the array read 5 to set 6, read 4 to set 5, read 3 to set 4...
  

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void setup() {

  pinMode(analogSwitchPin, INPUT);  // A0, 5 pushbuttons, on a series of resistors
  pinMode(analogRedPin, INPUT);    // A1  center of a  10-k pot between 5v and ground
  pinMode(analogGreenPin, INPUT);  // A2    "
  pinMode(analogBluePin, INPUT);   // A3    "
  pinMode(datapin, OUTPUT);   // should be 11, for the MaceTech ShiftBrite Shield
  pinMode(latchpin, OUTPUT);   // should be 9...
  pinMode(enablepin, OUTPUT);   // should be 10...
  pinMode(clockpin, OUTPUT);  // should be 13...
  SPCR = (1<<SPE)|(1<<MSTR)|(0<<SPR1)|(0<<SPR0);
  digitalWrite(latchpin, LOW);
  digitalWrite(enablepin, LOW);
  
  // set up the LCD's number of columns and rows: 
  lcd.begin(20, 2);
  lcd.clear();      // start with a blank screen, cursor at the upper-left.
  delay (500);
  // Print a message to the LCD.
  lcd.setCursor(0, 0);   // set cursor to column 0, row 0 (the first row)
  lcd.print("Shiftbrite Tuner");  // change this text to whatever you like. 
  lcd.setCursor(0, 1);           // set cursor to column 0, row 1
  lcd.print("Zonker Harris   v0.6");  
  delay (1000); // Pause a second, to see the splash screen text...
  // Write all black to the Shiftbrite array...
  for (int pixel = 0; pixel < (NumLEDs) + 1; pixel++)  
  {
    LEDChannels[pixel][0] = 0;
    LEDChannels[pixel][1] = 0;
    LEDChannels[pixel][2] = 0;
    WriteLEDArray();
  }
  // Run a simple rainbow "lamp test" on the Shiftbrites...
  int icDelay = 200;
  int myBaseColor = 21;
  LEDChannels[0][0] = myColors[myBaseColor];
  LEDChannels[0][1] = myColors[myBaseColor + 1];
  LEDChannels[0][2] = myColors[myBaseColor + 2];
  WriteLEDArray();
  delay(icDelay);
  LEDChannels[0][0] = 0;
  LEDChannels[0][1] = 0;
  LEDChannels[0][2] = 0;
  WriteLEDArray();
  myBaseColor = 36;
  LEDChannels[1][0] = myColors[myBaseColor];
  LEDChannels[1][1] = myColors[myBaseColor + 1];
  LEDChannels[1][2] = myColors[myBaseColor + 2];
  WriteLEDArray();
  delay(icDelay);
  LEDChannels[1][0] = 0;
  LEDChannels[1][1] = 0;
  LEDChannels[1][2] = 0;
  WriteLEDArray();
  myBaseColor = 51;
  LEDChannels[2][0] = myColors[myBaseColor];
  LEDChannels[2][1] = myColors[myBaseColor + 1];
  LEDChannels[2][2] = myColors[myBaseColor + 2];
  WriteLEDArray();
  delay(icDelay);
  LEDChannels[2][0] = 0;
  LEDChannels[2][1] = 0;
  LEDChannels[2][2] = 0;
  WriteLEDArray();
  myBaseColor = 66;
  LEDChannels[3][0] = myColors[myBaseColor];
  LEDChannels[3][1] = myColors[myBaseColor + 1];
  LEDChannels[3][2] = myColors[myBaseColor + 2];
  WriteLEDArray();
  delay(icDelay);
  LEDChannels[3][0] = 0;
  LEDChannels[3][1] = 0;
  LEDChannels[3][2] = 0;
  WriteLEDArray();
  myBaseColor = 81;
  LEDChannels[4][0] = myColors[myBaseColor];
  LEDChannels[4][1] = myColors[myBaseColor + 1];
  LEDChannels[4][2] = myColors[myBaseColor + 2];
  WriteLEDArray();
  delay(icDelay);
  LEDChannels[4][0] = 0;
  LEDChannels[4][1] = 0;
  LEDChannels[4][2] = 0;
  WriteLEDArray();
  myBaseColor = 96;
  LEDChannels[5][0] = myColors[myBaseColor];
  LEDChannels[5][1] = myColors[myBaseColor + 1];
  LEDChannels[5][2] = myColors[myBaseColor + 2];
  WriteLEDArray();
  delay(icDelay);
  LEDChannels[5][0] = 0;
  LEDChannels[5][1] = 0;
  LEDChannels[5][2] = 0;
  WriteLEDArray();
  button = 0;
  lcd.clear();   // start with a blank screen, cursor at the upper-left.
  delay(icDelay);
}

void loop() {
  // Get the reading from the pushbuttons
  int switches = analogRead(analogSwitchPin);
    
if (arrayGroup > 0) {   // Are we reviewing the group/set?
  lcd.clear();  // Clear the screen, put the cursor to the left of the top row
  lcd.setCursor(0, 0);  // Set the cursor to (column, row), starting with zero
  lcd.print("Group: ");
  lcd.print(arrayGroup);
  lcd.print(" Set: ");
  lcd.print(arraySet);
  colorSet = ((arrayGroup -1) * 15) + ((arraySet - 1) * 3);
  
  // read in the three analog values 
  redValue = myColors[colorSet];
  greenValue = myColors[colorSet + 1];
  blueValue = myColors[colorSet + 2];
}

if (arrayGroup == 0) {   // Are we tuning the color manually?
  lcd.clear();  // Clear the screen, put the cursor to the left of the top row
  lcd.setCursor(0, 0);  // Set the cursor to (column, row), starting with zero
  lcd.print("Tuning with knobs...");
  // read in the three analog values 
  redValue = analogRead (analogRedPin);
  greenValue = analogRead (analogGreenPin);
  blueValue = analogRead (analogBluePin);
}

  LEDChannels[5][0] = redValue;
  LEDChannels[5][1] = greenValue;
  LEDChannels[5][2] = blueValue;
  WriteLEDArray(); 

/*
lcd.setCursor(0, 1);  // row 1 = second row
  lcd.print("colorSet: ");
  lcd.print(colorSet);
*/

// Print a message to the LCD.
  lcd.setCursor(0, 1);  // row 1 = second row
  lcd.print("R:");
  lcd.print(redValue);
  lcd.print(" G:");
  lcd.print(greenValue);
  lcd.print(" B:");
  lcd.print(blueValue);
  
  delay (100);

  if ((switches > 120) && (switches < 125))  // Group - 1
  {
    button = 1;
    if (arrayGroup > 0) { arrayGroup = (arrayGroup - 1); }
    // zero = tuning. 1 = myColors group 0, 2 = myColors group 1, ...
  }
  if ((switches > 135) && (switches < 140))  // Group + 1
  {
    button = 2;
    if (arrayGroup < (colorMax + 1)) { arrayGroup++; }
  }
  if ((switches > 190) && (switches < 195))  // Set - 1 (Lower is BRIGHTER!)
  {
    button = 3;
    if (arraySet > 1) { arraySet = (arraySet - 1); }
  }
  if ((switches > 230) && (switches < 235))  // Set + 1  (Higher is DIMMER!)
  {
    button = 4;
    if (arraySet < 5) { arraySet++; }
  }
  if ((switches > 465) && (switches < 470))  // Shift the array +1
  {
    button = 5;
    lcd.setCursor(0, 0);  // 1st column of 1 row...
    lcd.print("Shifting the Array");
    delay(200);
    for (int pixel = 0; pixel < 6; pixel++)  
    {
      int element = 6 - pixel;
      LEDChannels[pixel][0] = LEDChannels[pixel+1][0];
      LEDChannels[pixel][1] = LEDChannels[pixel+1][1];
      LEDChannels[pixel][2] = LEDChannels[pixel+1][2];
    }
  }
  if (switches < 10)  // Default, no button is pressed, write to LCD & LED
  {
    button = 0;
  }
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
int randomSmall() {
  int result;
  result = (random(1, 6) * 15);
  return result;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
int randomBig() {
  int result;
  result = (random(5, 34) * 3);
  return result;
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
int randomSpread() {
  int result;
  int mySpread = (-1, 1);
  result = (random(1,10) * 15) + mySpread;
  return result;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 The following code is from the original ShiftBrite demo code, by
 Garret Mace, at http://docs.macetech.com/doku.php/shiftbrite_2.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
 void SB_SendPacket() {

  if (SB_CommandMode == B01) {
    SB_RedCommand = 120;
    SB_GreenCommand = 100;
    SB_BlueCommand = 100;
  }

  SPDR = SB_CommandMode << 6 | SB_BlueCommand>>4;
  while(!(SPSR & (1<<SPIF)));
  SPDR = SB_BlueCommand<<4 | SB_RedCommand>>6;
  while(!(SPSR & (1<<SPIF)));
  SPDR = SB_RedCommand << 2 | SB_GreenCommand>>8;
  while(!(SPSR & (1<<SPIF)));
  SPDR = SB_GreenCommand;
  while(!(SPSR & (1<<SPIF)));
}

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
void WriteLEDArray() {

  SB_CommandMode = B00; // Write to PWM control registers
  for (int h = 0;h<NumLEDs;h++) {
    SB_RedCommand = LEDChannels[h][0];
    SB_GreenCommand = LEDChannels[h][1];
    SB_BlueCommand = LEDChannels[h][2];
    SB_SendPacket();
  }

  delayMicroseconds(15);
  digitalWrite(latchpin,HIGH); // latch data into registers
  delayMicroseconds(15);
  digitalWrite(latchpin,LOW);

  SB_CommandMode = B01; // Write to current control registers
  for (int z = 0; z < NumLEDs; z++) SB_SendPacket();
  delayMicroseconds(15);
  digitalWrite(latchpin,HIGH); // latch data into registers
  delayMicroseconds(15);
  digitalWrite(latchpin,LOW);
}


// * * * * * * * * * * < End of File > * * * * * * * * * * * * * * *

