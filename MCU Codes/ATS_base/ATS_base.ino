#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <ESP32Servo.h>    //  library to control the brushless motor
#include <LiquidCrystal_I2C.h>

// Provide the token generation process info.
#include <addons/TokenHelper.h>

// Provide the RTDB payload printing info and other helper functions.
#include <addons/RTDBHelper.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "mustechELECTRONICS LAB"
#define WIFI_PASSWORD "123456789"

// For the following credentials, see examples/Authentications/SignInAsUser/EmailPassword/EmailPassword.ino

/* 2. Define the API Key */
#define API_KEY "AIzaSyBXgQctCZUUdRFD77FMtT5Jkw7Fb35YKUk"

/* 3. Define the RTDB URL */
#define DATABASE_URL "https://iotvoice-9d398-default-rtdb.firebaseio.com" //<databaseName>.firebaseio.com or <databaseName>.<region>.firebasedatabase.app

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "apms2105@gmail.com"
#define USER_PASSWORD "1234567890"

// Define Firebase Data object
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;


// set the LCD number of columns and rows
int lcdColumns = 16;
int lcdRows = 2;

// set LCD address, number of columns and rows
// if you don't know your display address, run an I2C scanner sketch
LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);  

float amplitude_currentI;               //amplitude current
float effective_valueI;  
 float ACC;
 
int RELAY_A=13;
int RELAY_B=12;
#define SENSORI 32
int MAINS=34;
int GEN=25;
int INVERTER=27;

int MAINSState;
int GENState;
int INVERTERState;
//int buzz=4;

void setup() {
lcd.init();
lcd.backlight();
pinMode(MAINS,INPUT);
pinMode(GEN,INPUT);
pinMode(INVERTER,INPUT);

pinMode(RELAY_A,OUTPUT);
pinMode(RELAY_B,OUTPUT);
pinMode(SENSORI, INPUT);

 //wifi and firebase setup
       WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");
  unsigned long ms = millis();
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Serial.printf("Firebase Client v%s\n\n", FIREBASE_CLIENT_VERSION);

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;


  /* Assign the callback function for the long running token generation task */
  config.token_status_callback = tokenStatusCallback; // see addons/TokenHelper.h

  Firebase.begin(&config, &auth);

  Firebase.reconnectWiFi(true);

//pinMode(buzz,OUTPUT);
//lcd.setCursor(0, 0);
  lcd.print("SMART ATS");
  lcd.setCursor(0, 1);
lcd.print("SELECTOR");
 delay(3500);
 
 lcd.clear();
}

void loop() {
int sensor_maxI;
    sensor_maxI = getMaxValueI();
    ACC = map(sensor_maxI,0,324,0,5)-(1);
  
    amplitude_currentI=(float)(sensor_maxI-512)/1024*5/185*1000;// for 5A mode,you need to modify this with 20 A and 30A mode; 
    effective_valueI=amplitude_currentI/1.414; 
  

MAINSState = digitalRead(MAINS);
GENState = digitalRead(GEN);
INVERTERState = digitalRead(INVERTER);

  if((INVERTERState==LOW)&&(MAINSState==HIGH)&&(GENState==HIGH)){

    Firebase.RTDB.setString(&fbdo, "/ATS/current", "Inverter");
    lcd.clear();
    lcd.setCursor(0, 0);
  lcd.print("INVERTER");
  lcd.setCursor(0, 1);
lcd.print("CURRENT : ");
 lcd.print(ACC);
 delay(1000);
    digitalWrite(RELAY_A,LOW);
  digitalWrite(RELAY_B,LOW);
  
   }

  else if((INVERTERState==HIGH)&&(MAINSState==HIGH)&&(GENState==LOW)){
      
Firebase.RTDB.setString(&fbdo, "/ATS/current", "Generator");
lcd.clear();
    lcd.setCursor(0, 0);
  lcd.print("GEN.");
  lcd.setCursor(0, 1);
lcd.print("CURRENT : ");
 lcd.print(ACC);
 delay(1000);
    digitalWrite(RELAY_A,HIGH);
 // digitalWrite(Rblue,LOW);
  digitalWrite(RELAY_B,LOW);
 
  }
 else if((INVERTERState==HIGH)&&(MAINSState==LOW)&&(GENState==HIGH)){

   Firebase.RTDB.setString(&fbdo, "/ATS/current", "Mains");
   lcd.clear();
    lcd.setCursor(0, 0);
  lcd.print("MAINS");
  lcd.setCursor(0, 1);
lcd.print("CURRENT : ");
 lcd.print(ACC);
 delay(1000);
      digitalWrite(RELAY_A,HIGH);
  digitalWrite(RELAY_B,HIGH);
  }


  else if((INVERTERState==HIGH)&&(MAINSState==LOW)&&(GENState==LOW)){
     
     Firebase.RTDB.setString(&fbdo, "/ATS/current", "Mains");
lcd.clear();
lcd.setCursor(0, 0);
  lcd.print("MAINS");
  lcd.setCursor(0, 1);
lcd.print("CURRENT : ");
 lcd.print(ACC);
 delay(1000);
   digitalWrite(RELAY_A,HIGH);
  digitalWrite(RELAY_B,HIGH);
 
  }

  else if((MAINSState==HIGH)&&(GENState==LOW)){

Firebase.RTDB.setString(&fbdo, "/ATS/current", "Generator"); 
lcd.clear();
lcd.setCursor(0, 0);
  lcd.print("GEN PHASE");
  lcd.setCursor(0, 1);
lcd.print("CURRENT : ");
 lcd.print(ACC);
 delay(1000);
    digitalWrite(RELAY_A,HIGH);
  digitalWrite(RELAY_B,LOW);
  
  } 
  
  else if((INVERTERState==LOW)&&(MAINSState==LOW)&&(GENState==LOW)){

Firebase.RTDB.setString(&fbdo, "/ATS/current", "Inverter");
lcd.clear();
lcd.setCursor(0, 0);
  lcd.print("INVERTER");
  lcd.setCursor(0, 1);
lcd.print("CURRENT : ");
 lcd.print(ACC);
 delay(1000);
    digitalWrite(RELAY_A,LOW);
  digitalWrite(RELAY_B,LOW);
  
  }

   else if((INVERTERState==HIGH)&&(MAINSState==HIGH)&&(GENState==HIGH)){

Firebase.RTDB.setString(&fbdo, "/ATS/current", "Power Outage");
lcd.clear();
lcd.setCursor(0, 0);
  lcd.print("POWER OUTAGE");
  lcd.setCursor(0, 1);
lcd.print("CHECK CONNECTIONS");
 delay(1000);
  }
   else{
     Firebase.RTDB.setString(&fbdo, "/ATS/current", "Power Outage");
    lcd.clear();
lcd.setCursor(0, 0);
  lcd.print("POWER OUTAGE");
  lcd.setCursor(0, 1);
lcd.print("CHECK CONNECTIONS");
 delay(1000);
  }


  senddata(ACC);
}

 int getMaxValueI()
{
    int sensorValueI;             //value read from the sensor
    int sensorMaxI = 0;
    uint32_t start_timeI = millis();
    while((millis()-start_timeI) < 1000)//sample for 1000ms
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


void senddata(float b){

  Firebase.RTDB.setFloat(&fbdo, "/ATS/CU", b);

}

