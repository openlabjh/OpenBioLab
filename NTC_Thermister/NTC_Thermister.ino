#define SERIESRESISTOR 10000    
#define THERMISTORPIN A0 
#define R0 10000
#define T0 25 
#define B 3950 
void setup(void) {
  Serial.begin(9600);
}
 
void loop(void) {
  float reading;
 
  reading = analogRead(THERMISTORPIN);
 
  Serial.print("Analog reading "); 
  Serial.println(reading);
 
  // convert the value to resistance
  reading = (1023 / reading)  - 1;     // (1023/ADC - 1) 
  reading = SERIESRESISTOR / reading;  // 10K / (1023/ADC - 1)
  Serial.print("Thermistor resistance "); 
  Serial.println(reading);

  float temperature;
  temperature = reading / SERIESRESISTOR; // R/R0
  temperature = log(temperature);
  temperature = temperature / B;
  temperature = temperature + 1.0 / (T0 + 273.15);
  temperature = 1.0 / temperature;
  temperature -= 273.15 ;// kelvins to C
  Serial.println(temperature);
  delay(1000);
}
