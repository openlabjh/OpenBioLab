// Libraries
#include <math.h>
#include <Wire.h>
#include "LiquidCrystal_I2C.h"

//Define LCD
LiquidCrystal_I2C lcd(0x3F, 16, 2);

// Define pins
#define ledPin 4
#define fanPin 5
#define heatPin 6

#define buttonPin1 11 // temperature up
#define buttonPin2 12  // temperature down

#define SERIESRESISTOR 10000
#define R0 10000
#define T0 25 //room temperature
#define B 3950 //the coefficient of the thermistor

// Define variables

int targetTemp = 18;   // Initial target temperature
int button1State = 0;
int button2State = 0;
int val;
double temp;
boolean isCheck;

//  Time variables

long switch_time = 2000; // switch mosfet on or off once every 3 seconds
long begin_switch_time = 0;
long temp_time = 2000; // read temperature once every 2 seconds
long begin_temp_time = 0;

int thermistorPin = A0;

void setup() {
  Serial.begin(115200);
  Serial.println(F("Open Bio Lab Academy 2017 Incubator"));

  pinMode(ledPin, OUTPUT);
  pinMode(fanPin, OUTPUT);
  pinMode(heatPin, OUTPUT);
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);

  // Set time
  begin_switch_time = millis();

  // Initialize I2C
  Wire.begin();

  // Initialize the LCD and print a message
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(F("Open Bio Lab"));
  lcd.setCursor(0, 1);
  lcd.print(F("Academy 2017"));
  delay(3000);
  lcd.clear();

  // The ,LED Bar, fan is always on
  digitalWrite(ledPin, HIGH);
  digitalWrite(fanPin, HIGH);
}


double Thermister(float RawADC) {
  RawADC = (1023 / RawADC) - 1;
  RawADC = SERIESRESISTOR / RawADC;  // 10K / (1023/ADC - 1)

  float Temp;
  Temp = RawADC / R0; // R/R0
  Temp = log(Temp);
  Temp = Temp / B;
  Temp = Temp + 1.0 / (T0 + 273.15);
  Temp = 1.0 / Temp;
  Temp -= 273.15 ;// kelvins to C
  return Temp;
}

void loop() {
  isCheck = digitalRead(heatPin);
  Serial.println(isCheck);
  if ((millis() - begin_temp_time) > temp_time) {

    val = analogRead(thermistorPin);
    temp = Thermister(val);
    if (isCheck == 0) {
      temp -= 2;
    }
    begin_temp_time = millis();
  }
  if ((millis() - begin_switch_time) > switch_time) {
    if (temp < targetTemp) {
      digitalWrite(heatPin, HIGH);
      begin_switch_time = millis();
      isCheck = true;
    }
    else {
      digitalWrite(heatPin, LOW);
      begin_switch_time = millis();
      isCheck = false;
    }
  }

  button1State = digitalRead(buttonPin1);
  button2State = digitalRead(buttonPin2);

  if (button1State == HIGH) {
    ++targetTemp;
    if (targetTemp > 50) targetTemp = 50;
    delay(500);
  }
  else if (button2State == HIGH) {
    --targetTemp;
    if (targetTemp < 0) targetTemp = 0;
    if (targetTemp == temp && isCheck == 1) {
      temp -= 2;
    }
    delay(500);
  }

  button1State = 0;
  button2State = 0;

  Serial.print(F("Temp: "));
  Serial.print(temp);
  Serial.print(F(" Target temp: "));
  Serial.println(targetTemp);

  displayTemps(constrain(temp, 0, 99), constrain(targetTemp, 0, 99));
}

void displayTemps(int tempToDisplay, int targetToDisplay) {
  lcd.setCursor(0, 0);
  lcd.print(F("Temp "));
  lcd.setCursor(5, 0);
  lcd.print(tempToDisplay);
  lcd.print(F(" C"));
  lcd.setCursor(0, 1);
  lcd.print(F("Target "));
  lcd.setCursor(7, 1);
  lcd.print(targetToDisplay);
  lcd.print(F(" C"));
}
