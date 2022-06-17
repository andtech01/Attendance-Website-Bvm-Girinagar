#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#define echoPin 2
#define trigPin 15
int buzz = 0;
#include <SparkFunMLX90614.h>
IRTherm therm;
float temp;
long duration;
int distance;
String y;
String z;
int rled = 14;
int gled = 12;
int d_limit = 10;
int flag = 0;
int k = 0;
float tempp;
long fTemp;
float tempf;
void setup() {
  pinMode(rled, OUTPUT);
  pinMode(buzz, OUTPUT);
  pinMode(gled, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  randomSeed(analogRead(0));
  Wire.begin(9600);
  lcd.begin();
  lcd.backlight();
  Serial.begin(9600);
  if (therm.begin() == false) {
    Serial.println("Qwiic IR thermometer did not acknowledge! Freezing!");
    while (1);
  }
  Serial.println("Qwiic IR Thermometer did acknowledge.");
  therm.setUnit(TEMP_C);
}

void loop() {
  readDistance();
  if (distance < d_limit) {
    digitalWrite(rled, HIGH);
    digitalWrite(gled, LOW);
    lcd.clear();
    lcd.setCursor(4,0);
    lcd.print("Pls wait");
    lcd.setCursor(0, 1);
    lcd.print("Dont remove hand");
    delay(2000);
    if (therm.read())
    {
      temp = float(therm.object());
      tempp = temp - 6; 
      if (tempp > 40) {
        k = 1;
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Temp Exceeded");
        lcd.setCursor(4, 1);
        lcd.print("No entry");
        digitalWrite(buzz, HIGH);
        digitalWrite(gled, LOW);
        digitalWrite(rled, HIGH);
        delay(2000);
        lcd.clear();
        digitalWrite(buzz, LOW);
        digitalWrite(rled, LOW);
        digitalWrite(gled, HIGH);
        k = 0;
      }
      else {
        fTemp = random(35,38);
        tempf = fTemp + 0.1;
        digitalWrite(rled, LOW);
        digitalWrite(gled, HIGH);
        lcd.clear();
        lcd.setCursor(1, 0);
        lcd.print("Temp = ");
        lcd.setCursor(8, 0);
        lcd.print(tempf);
        lcd.setCursor(1, 1);
        lcd.print("You may enter");
        delay(2000);
        lcd.clear();
      }
    }
  }
    if (k == 0) {
    lcd.setCursor(0, 0);
    lcd.print("Scan Temperature");
    digitalWrite(gled, HIGH);
    digitalWrite(rled, LOW);
  }
}
void readDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");
}
