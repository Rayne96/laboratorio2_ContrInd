#include <PID_v1.h>
#include <Wire.h>
#include <LiquidCrystal.h>
#include <util/delay.h>

// Creates an LCD object. Parameters: (rs, enable, d4, d5, d6, d7)
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int sensorPin = A0;
int botonUp = 6;
int botonDown = 7;
int relePin = 8;
int salidaPWM = 9;

//Define Variables we'll be connecting to
double Setpoint, Input, Output;
double Kp = 2.0, Ki = 5.0, Kd = 1.0;

//Specify the links and initial tuning parameters
PID myPID(&Input, &Output, &Setpoint,Kp,Ki,Kd, DIRECT);

void setup()
{
  
  //Esto siempre es así por las columnas y filas del display 
  lcd.begin(16, 2);
  lcd.clear();

  pinMode(botonUp, INPUT_PULLUP);
  pinMode(botonDown, INPUT_PULLUP);
  pinMode(relePin, OUTPUT);
  Serial.begin(9600);
  myPID.SetMode(AUTOMATIC);
  myPID.SetOutputLimits(0, 255); // PWM de 0 a 255

}

void loop() 
{
  // Print a message to the LCD.
  lcd.print(" Hello world!");

  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // Print a message to the LCD.
  lcd.print(" LCD Tutorial");

  _delay_ms(1000);
  
  lcd.clear();
}
