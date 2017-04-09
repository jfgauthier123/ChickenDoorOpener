#include <Arduino.h>
//Below is the code. It takes into account the day of the year and adjusts sunrise and sunset. The door opens one hour after sunrise and closes one hour after sunset.

/*
* ChickenCoopDoor.pde
*
* Close door at sunset plus
* Open door at sunrise plus
*
* Once a day just after midnight calculate sunrise and sunset times for the day
*
* Edwin A. Pell III 06/29/2010
*
*/

#include <Wire.h>
#include <Math.h>
#include <LiquidCrystal.h>
#define RTC_ADDR 0x68


int x = 0;
int currx = 1023;
String btnStr = "None";


int current_day;
int door_opened;
int door_closed;
int sunrise; // in minutes from midnight blabla
int sunset;  // in minutes from midnight

int openclose_a = 2;
int openclose_b = 3;

int second;
int minutex;
int hour;
int dayOfWeek;
int dayOfMonth;
int month;
int year;

// Configuration du LCD
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);


void setup()
{
// set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Porte de poule!");

 pinMode(openclose_a, OUTPUT);
 pinMode(openclose_b, OUTPUT);

 digitalWrite(openclose_a, LOW);
 digitalWrite(openclose_b, LOW);

 door_opened = 1;
 door_closed = 0;

 Wire.begin();
 ReadTime();
 int current_day = dayOfMonth;
 getRiseSet();

}

// Convert normal decimal numbers to binary coded decimal
byte decToBcd(byte val)
{
 return ( (val/10*16) + (val%10) );
}

// Convert binary coded decimal to normal decimal numbers
byte bcdToDec(byte val)
{
 return ( (val/16*10) + (val%16) );
}

void OpenDoor()
{
 lcd.clear();
 lcd.setCursor(0,0);
 lcd.print("Ouverture porte");
 digitalWrite(openclose_a, HIGH);
 digitalWrite(openclose_b, LOW);
 delay(9000);
 digitalWrite(openclose_a, LOW);
 digitalWrite(openclose_b, LOW);
 lcd.clear();
 lcd.setCursor(0,0);
 lcd.print("porte ouverte!");
 door_opened = 1;
}

void CloseDoor()
{
 lcd.clear();
 lcd.setCursor(0,0);
 lcd.print("Fermeture porte");
 digitalWrite(openclose_a, HIGH);
 digitalWrite(openclose_b, HIGH);
 delay(9000);
 digitalWrite(openclose_a, LOW);
 digitalWrite(openclose_b, LOW);
 lcd.clear();
 lcd.setCursor(0,0);
 lcd.print("porte fermee!");
 door_closed = 1;
}

void getRiseSet()
{
 sunrise = int(350 + (90*cos(((((month-1)*30.5)+dayOfMonth)+8)/58.1)));
 sunset = int(1075 + (90*sin(((((month-1)*30.5)+dayOfMonth)-83)/58.1)));
}

void NewDay()
{
 current_day = dayOfMonth;
 getRiseSet();
 door_opened = 0;
 door_closed = 0;
}

void printTime() {
  char buffer[3];
  const char* AMPM = 0;
  ReadTime();
  lcd.clear();
  lcd.setCursor(0,0);

  lcd.print(dayOfMonth);
  lcd.print("/");
  lcd.print(month);
  lcd.print("/");
  lcd.print(year);
  lcd.print(" ");
  if (hour > 12) {
    hour -= 12;
    AMPM = " PM";
  }
  else AMPM = " AM";
  lcd.print(hour);
  lcd.print(":");
  sprintf(buffer, "%02d", minutex);
  lcd.print(buffer);
  lcd.print(AMPM);

}

void ReadTime()
{
 Wire.beginTransmission(RTC_ADDR);   // Open I2C line in write mode
 Wire.write(0x00);                              // Set the register pointer to (0x00)
 Wire.endTransmission();                       // End Write Transmission

 Wire.requestFrom(RTC_ADDR, 7);      // Open the I2C line in send mode

 second     = bcdToDec(Wire.read() & 0x7f); // Read seven bytes of data
 minutex    = bcdToDec(Wire.read());
 hour       = bcdToDec(Wire.read() & 0x3f);
 dayOfWeek  = bcdToDec(Wire.read());
 dayOfMonth = bcdToDec(Wire.read());
 month      = bcdToDec(Wire.read());
 year       = bcdToDec(Wire.read());
}



void  loop()
{
 printTime();
 delay(1000);
 x = analogRead(A0); // the buttons are read from the analog0 pin

  // Check if x has changed
  if ((x != 1023) && (x != currx)){

    currx = x;

    if (currx > 739 && currx < 745){

       btnStr="Force Open";
       lcd.setCursor(0,1);
       lcd.print(btnStr);
       delay(500);
       OpenDoor();

    } else if (currx > 500 && currx < 510){

       btnStr="Force Close";
       lcd.setCursor(0,1);
       lcd.print(btnStr);
       delay(500);
       CloseDoor();

    } else if (currx < 10){

      btnStr="Right";

    } else if (currx > 140 && currx < 150){

      btnStr="Up";

    } else if (currx > 320 && currx < 365){

      btnStr="Down";

    }

  }
   //delay(300000);
 ReadTime();
 if (current_day!=dayOfMonth)
 {
   NewDay();
 }

 int current_time = (hour*60)+minutex;

 if (current_time > (sunrise+60))
 {
   if (door_opened == 0)
   {
     OpenDoor();
   }
 }

 if (current_time > (sunset+60))
 {
   if (door_closed == 0)
   {
     CloseDoor();
   }
 }
}
