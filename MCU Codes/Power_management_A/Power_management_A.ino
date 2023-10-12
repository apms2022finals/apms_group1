#include <LiquidCrystal_I2C.h>
#include <FirebaseESP32.h>
#include <WiFi.h>

#define ssid "A.F.I"
#define password "123456789"
#define FIREBASE_HOST "https://power-management-c9fa4-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "AIzaSyBuSMg1AfyyGjQgo_gQWbSgkzSSsrtJwes"
FirebaseData firebaseData;

// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);
#define SENSOR 25 // Analog input pin that sensor is attached to
#define SENSORI 32

const int relay_pin = 13;
const int relay_pin1 = 12;
const int relay_pin2 = 14;
float amplitude_currentV; // amplitude current
float effective_valueV;

float amplitude_currentI; // amplitude current
float effective_valueI;
float ACV;
float ACC;

void setup() {
  digitalWrite (relay_pin, HIGH);
  digitalWrite (relay_pin1, HIGH);
  digitalWrite (relay_pin2, HIGH);
  pinMode(SENSOR, INPUT);
  pinMode(SENSORI, INPUT);
  // initialize LCD
  lcd.init();
  // turn on LCD backlight
  lcd.backlight();
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH); // Initialize Firebase

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Connecting...");
  }

  // Successfully connected to Wi-Fi
  Serial.println("Connected to WiFi!");
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Connected");
  delay(1000);

  lcd.print("POWER MAGN.");
  lcd.setCursor(0, 1);
  lcd.print("APARTMENT B");
  delay(3000);
}

void loop() {
  int sensor_maxV;
  sensor_maxV = 0.1 * getMaxValueV();
  ACV = map(sensor_maxV, 0, 324, 0, 240) - (1);

  amplitude_currentV = (float)(sensor_maxV - 512) / 1024 * 5 / 185 * 1000000; // for 5A mode, you need to modify this with 20 A and 30A mode;
  effective_valueV = amplitude_currentV / 1.414;
  // minimum_current=1/1024*5/185*1000000/1.414=18.7(mA)
  // Only for sinusoidal alternating current
  //////########################################################

  int sensor_maxI;
  sensor_maxI = getMaxValueI();
  ACC = map(sensor_maxI, 0, 324, 0, 5) - (1);
  amplitude_currentI = (float)(sensor_maxI - 512) / 1024 * 5 / 185 * 1000; // for 5A mode, you need to modify this with 20 A and 30A mode;
  effective_valueI = amplitude_currentI / 1.414;
  // minimum_current=1/1024*5/185*1000000/1.414=18.7(mA)
  // Only for sinusoidal alternating current
  //////########################################################

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("VOLT. = ");
  lcd.print(sensor_maxV);
  lcd.setCursor(0, 1);
  lcd.print("CURRENT= ");
  lcd.print(ACC);

  //lcd.print("TEMP: ");
  //lcd.print(t);
  //lcd.print("C");
  delay(1000);

  Firebase.setFloat(firebaseData,"/APARTMENT A/VOLTAGE", sensor_maxV);
  Firebase.setFloat(firebaseData, "/APARTMENT A/CURRENT", ACC);

}

int getMaxValueV() {
  int sensorValueV; // value read from the sensor
  int sensorMaxV = 0;
  uint32_t start_timeV = millis();
  while ((millis() - start_timeV) < 1000) // sample for 1000ms
  {
    sensorValueV = analogRead(SENSOR);
    if (sensorValueV > sensorMaxV)
    {
      /*record the maximum sensor value*/
      sensorMaxV = sensorValueV;
    }
  }
  return sensorMaxV;
}

int getMaxValueI() {
  int sensorValueI; // value read from the sensor
  int sensorMaxI = 0;
  uint32_t start_timeI = millis();
  while ((millis() - start_timeI) < 1000) // sample for 1000ms
  {
    sensorValueI = analogRead(SENSORI);
    if (sensorValueI > sensorMaxI)
    {
      /*record the maximum sensor value*/
      sensorMaxI = sensorValueI;
    }
  }
  return sensorMaxI;
}
