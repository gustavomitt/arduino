#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>  // F Malpartida's NewLiquidCrystal library


#define I2C_ADDR    0x20  // Define I2C Address for controller
#define BACKLIGHT_PIN  15
#define En_pin  6
#define Rw_pin  5
#define Rs_pin  4
#define D4_pin  11
#define D5_pin  12
#define D6_pin  13
#define D7_pin  14

#define  LED_OFF  0
#define  LED_ON  1
LiquidCrystal_I2C  lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);

void setup() 
{
  lcd.begin (16,2);  // initialize the lcd
// Switch on the backlight
  lcd.setBacklightPin(BACKLIGHT_PIN,NEGATIVE);
  lcd.setBacklight(LED_ON);
}

void loop()  
{

// Reset the display 
  lcd.clear();
  delay(1000);
  lcd.home();
 
// Print on the LCD
  lcd.backlight();  
  lcd.setCursor(0,0); 
  lcd.print("Hello, world!");
  delay(8000);
}
