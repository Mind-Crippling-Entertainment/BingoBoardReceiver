#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <string.h>
#include <espnow.h>
#include <Adafruit_PWMServoDriver.h>




LiquidCrystal_I2C lcd(0x27,20,4);

const int screenOnDuration = 4000;
unsigned long screenSaverRunTime = 0;
const String version = "1.2.2";

void lcdDisplay(String, String, String, String);
void ScreenSaver(void);

Adafruit_PWMServoDriver pwm0 = Adafruit_PWMServoDriver(0x41); // Pins 0-15 B
Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x42); // Pins 0-15 I
Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(0x43); // Pins 0-15 N
Adafruit_PWMServoDriver pwm3 = Adafruit_PWMServoDriver(0x44); // Pins 0-15 G
Adafruit_PWMServoDriver pwm4 = Adafruit_PWMServoDriver(0x45); // Pins 0-15 O


// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
    char a[32];
    int b;
    float c;
    String d;
    bool e;
} struct_message;

// Create a struct_message called myData
struct_message myData;

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&myData, incomingData, sizeof(myData));
  lcdDisplay(
    "Bytes received: " + len,
   "Char: " + String(myData.a),
    "Int: " + String(myData.b),
   "Float: " + String(myData.c)
  );
  
  Serial.print("String: ");
  Serial.println(myData.d);
  Serial.println(myData.e);
  
  DisplayLight(myData.b);
} 


void setup()
{
  Wire.begin(D1,D2);
  lcd.init();

  pwm0.begin();
  pwm0.setOscillatorFrequency(27000000);
  pwm0.setPWMFreq(1600);  // This is the maximum PWM frequency

  pwm1.begin();
  pwm1.setOscillatorFrequency(27000000);
  pwm1.setPWMFreq(1600);  // This is the maximum PWM frequency

  pwm2.begin();
  pwm2.setOscillatorFrequency(27000000);
  pwm2.setPWMFreq(1600);  // This is the maximum PWM frequency

  pwm3.begin();
  pwm3.setOscillatorFrequency(27000000);
  pwm3.setPWMFreq(1600);  // This is the maximum PWM frequency

  pwm4.begin();
  pwm4.setOscillatorFrequency(27000000);
  pwm4.setPWMFreq(1600);  // This is the maximum PWM frequency


  Serial.begin(115200);
  Serial.println();
  Serial.print("ESP8266 Board MAC Address:  ");
  Serial.println(WiFi.macAddress());

  
  Serial.write("Conntected");
  
  lcdDisplay(
    "Bingo Display Board",
    "Board Version 1.0.2",
    "Firmware " + version,
    "By: Eko Illius"
  );
 
 delay(2000);

 // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);

}



void lcdDisplay(String line1 = "", String line2 = "", String line3 = "", String line4= "" )
{
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0, 1);
  lcd.print(line2);
  lcd.setCursor(0, 2);
  lcd.print(line3);
  lcd.setCursor(0, 3);
  lcd.print(line4);
  screenSaverRunTime = millis();
}

void ScreenSaver(void)
{
  if((millis() - screenSaverRunTime) > screenOnDuration){
    screenSaverRunTime = 0;
    lcd.clear();
    lcd.noBacklight();
  }
}

void DisplayLight(int light){

}

void loop() {
  ScreenSaver();
 }