#include <FirebaseESP8266.h>
#include  <ESP8266WiFi.h>
#include <time.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
#include <NTPClient.h>
#include <WiFiUdp.h>
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
String weekDays[7] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
String months[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
String currentDate;
#define ssid "Redmi"
#define password "dakh2014"
#define FIREBASE_HOST "attendance-system-57df7-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "RjAp7EGotybnx05ualxHdasj77qBpAiFSDoTZdHy"
#define echoPin 2
#define trigPin 15
int buzz = 0;
#include <SparkFunMLX90614.h>
IRTherm therm;
float temp;
long duration;
int distance;
FirebaseData firebaseData;
FirebaseData firebaseData2;
FirebaseData firebaseData3;
FirebaseJson json;
FirebaseJson json2;
FirebaseJson json3;
String x;
String formattedTime;
String y;
String z;
int rled = 14;
int gled = 12;
int laser = 13;
String Speed_Value ;
String newX;
int xLen;
int yLen;
int zLen;
String rfid;
String stdName;
String stdClass;
int d_limit = 10;
int timezone = 19800;
int dst = 0;
int k = 0;
int dayy;
int monthh;
int yearr;
int hourr;
int minn;
int secc;
int flag = 0;

void setup() {
  pinMode(rled, OUTPUT);
  pinMode(buzz, OUTPUT);
  pinMode(gled, OUTPUT);
  pinMode(laser, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  Wire.begin(9600);
  lcd.begin();
  lcd.backlight();
  Serial.begin(9600);
  WiFi.begin (ssid, password);
  if (therm.begin() == false) {
    Serial.println("IR thermometer did not acknowledge! Freezing!");
    while (1);
  }
  Serial.println("IR Thermometer did acknowledge.");
  therm.setUnit(TEMP_C);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  configTime(timezone, dst, "pool.ntp.org", "time.nist.gov");
  Serial.println("\nWaiting for Internet time");

  while (!time(nullptr)) {
    Serial.print("*");
    delay(1000);
  }
  Serial.println("\nTime response....OK");

}

void loop() {
  if (WiFi.status() != WL_CONNECTED)
  {
    flag = 0;
    Serial.println("Not Connected");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("No Connection");
    digitalWrite(rled, HIGH);
    digitalWrite(gled, LOW);
    digitalWrite(laser, LOW);
  }

  else
  {
    readRfid();
    readDistance();
    readTemp();
    getTimeNtp();
    flag = 1;
    digitalWrite(laser, HIGH);
    lcd.clear();
    if (k == 1) {
      digitalWrite(rled, LOW);
      digitalWrite(gled, HIGH);
      lcd.setCursor(2, 0);
      lcd.print("Tag Detected");
      lcd.setCursor(0, 1);
      lcd.print("Scan Temperature");
      if (distance < d_limit) {
        digitalWrite(rled, HIGH);
        digitalWrite(gled, LOW);
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Please wait");
        digitalWrite(rled, HIGH);
        digitalWrite(gled, LOW);
        if (temp > 40) {
          lcd.clear();
          lcd.setCursor(2, 0);
          lcd.print("Temp Exceeded");
          lcd.setCursor(4, 1);
          lcd.print("No entry");
          digitalWrite(buzz, HIGH);
          delay(2000);
          k = 0;
        }
        else {
          sendDataToFirebase();
          digitalWrite(rled, LOW);
          digitalWrite(gled, HIGH);
          lcd.clear();
          lcd.setCursor(1, 0);
          lcd.print("Temp = ");
          lcd.setCursor(7, 0);
          lcd.print(temp);
          lcd.setCursor(1, 1);
          lcd.print("You may enter");
          delay(1000);
          lcd.clear();
          k = 0;
        }
      }
    }
    else if ( k == 0) {
      lcd.setCursor(4, 0);
      lcd.print("Scan Tag");
      digitalWrite(buzz, LOW);
      digitalWrite(rled, LOW);
      digitalWrite(gled, HIGH);
    }
    else if (k == 2) {
      digitalWrite(rled, HIGH);
      digitalWrite(gled, LOW);
      lcd.setCursor(2, 0);
      lcd.print("Scan failed");
      lcd.setCursor(3, 1);
      lcd.print("Try again");
    }
  }
}

void readRfid() {
  if (flag == 1) {
    if (Serial.available() > 0) {
      rfid = Serial.readString();
      int rfidLen = rfid.length();
      if (rfidLen > 12) {
        k = 2;
        lcd.clear();
      }
      else {
        Serial.println("Rfid Id = " + rfid);
        lcd.clear();
        lcd.setCursor(2, 0);
        lcd.print("Please wait");
        digitalWrite(rled, HIGH);
        digitalWrite(gled, LOW);
        if (Firebase.getString(firebaseData, "/Student Data/" + rfid + "/Name"))
        {
          stdName = firebaseData.stringData();
          int l = stdName.length();
          stdName.remove((l - 1), 1);
          stdName.remove(0, 1);
          Serial.println(stdName);
        }
        if (Firebase.getString(firebaseData, "/Student Data/" + rfid + "/Class"))
        {
          stdClass = firebaseData.stringData();
          int l = stdClass.length();
          stdClass.remove((l - 1), 1);
          stdClass.remove(0, 1);
          Serial.println(stdClass);
        }
        k = 1;
      }
      lcd.clear();
    }
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
void readTemp() {
  if (therm.read())
  {
    temp = float(therm.object());
    Serial.println(temp);
  }
}
void getTimeNtp() {
  time_t now = time(nullptr);
  struct tm* p_tm = localtime(&now);
  dayy = p_tm->tm_mday;
  monthh = p_tm->tm_mon + 1;
  yearr = p_tm->tm_year + 1900;
  hourr = p_tm->tm_hour;
  minn = p_tm->tm_min;
  secc = p_tm->tm_sec;
  if (monthh < 10) {
    if (dayy < 10) {
      currentDate = "0" + String(dayy) + "-0" + String(monthh) + "-" + String(yearr);
    }
    else {
      currentDate = String(dayy) + "-0" + String(monthh) + "-" + String(yearr);
    }
  }
  else {
    if (dayy < 10) {
      currentDate = "0" + String(dayy) + "-0" + String(monthh) + "-" + String(yearr);
    }
    else {
      currentDate = String(dayy) + "-0" + String(monthh) + "-" + String(yearr);
    }
  }
  formattedTime = String(hourr) + ":" + String(minn) + ":" + String(secc);
  Serial.print(currentDate);
  Serial.print(" ");
  Serial.print(formattedTime);
}
void sendDataToFirebase() {
  if (Firebase.getString(firebaseData, "/Attendance/" + currentDate + "/" + stdClass))
  {
    x = firebaseData.stringData();
    Serial.println("X Raw = " + x);
    if (x == "null") {
      x.remove(0, 4);
      Serial.println("Null Val = " + x);
      x = x + stdName;
    }
    else {
      xLen = x.length();
      x.remove((xLen - 1), 1);
      x = x + "," + stdName;
      x.remove(0, 1);
      Serial.println("Final x = " + x);
    }
  } //Name
  if (Firebase.getString(firebaseData2, "/Temp/" + currentDate + "/" + stdClass + "Temp"))
  {
    y = firebaseData2.stringData();
    Serial.println("Y Raw = " + y);
    if (y == "null") {
      y.remove(0, 4);
      Serial.println("Y Null Val = " + y);
      y = y + temp;
    }
    else {
      yLen = y.length();
      y.remove((yLen - 1), 1);
      y = y + "," + temp;
      y.remove(0, 1);
      Serial.println("Final y = " + y);
    }
  } //Temp
  if (Firebase.getString(firebaseData3, "/Timings/" + currentDate + "/" + stdClass + "Timings"))
  {
    z = firebaseData3.stringData();
    Serial.println("Z Raw = " + z);
    if (z == "null") {
      z.remove(0, 4);
      Serial.println("Null Val = " + z);
      z = z + formattedTime;
    }
    else {
      zLen = z.length();
      z.remove((zLen - 1), 1);
      z = z + "," + formattedTime;
      z.remove(0, 1);
      Serial.println("Final z = " + z);
    }
  }
  setValue();
}

void setValue() {
  Speed_Value = x;
  json.set("/" + currentDate + "/" + stdClass, x);
  json2.set("/" + currentDate + "/" + stdClass + "Temp", y);
  json3.set("/" + currentDate + "/" + stdClass + "Timings", z);
  Firebase.setString(firebaseData, "/Attendance/" + currentDate + "/" + stdClass, x);
  Firebase.setString(firebaseData2, "/Temp/" + currentDate + "/" + stdClass + "Temp", y);
  Firebase.setString(firebaseData3, "/Timings/" + currentDate + "/" + stdClass + "Timings", z);
}
