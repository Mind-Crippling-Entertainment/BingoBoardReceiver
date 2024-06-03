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
void ToggleLight(int);
void ProcessInput(char);

bool lightStatus[81]; 

Adafruit_PWMServoDriver pwm0 = Adafruit_PWMServoDriver(0x41); // Pins 0-15 B
Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x42); // Pins 0-15 I
Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(0x43); // Pins 0-15 N
Adafruit_PWMServoDriver pwm3 = Adafruit_PWMServoDriver(0x44); // Pins 0-15 G
Adafruit_PWMServoDriver pwm4 = Adafruit_PWMServoDriver(0x45); // Pins 0-15 O


// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
    char a;
    
} struct_message;

// Create a struct_message called myData
struct_message myData;

// Callback function that will be executed when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&myData, incomingData, sizeof(myData));
  lcdDisplay("Bytes received: " + len, "Bingo Number: "  + String(myData.a), "", "" );
  ProcessInput(myData.a);

} 

int i = 0;

void setup()
{
  Wire.begin(D1,D2);
  lcd.init();
  
  while(i <= 81){
    lightStatus[i] = false;
    i++;
  }


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


void ProcessInput(char incomingData){
    switch (incomingData)
    {
      case 'A':
      break;
        
    
      case '*':
      break;

      default:
        ToggleLight(int(incomingData));
      break;
  }

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

void ToggleLight(int light){
  if(lightStatus[light] == false) {
   
   if(light <= 15){
      lightStatus[76] = true; //B Light Up    
      pwm0.setPWM(0, 4096, 0);
      
      lightStatus[light] = true; // Number Lit Up
      pwm0.setPWM(light, 4096, 0);
    }

     if(light >= 16 && light <= 30){
      lightStatus[77] = true; // I Light Up    
      pwm1.setPWM(0, 4096, 0);
      
      lightStatus[light] = true; // Number Lit Up
      pwm1.setPWM(light, 4096, 0);
    }

    if(light >= 31 && light <= 45){
      lightStatus[78] = true; // N Light Up    
      pwm2.setPWM(0, 4096, 0);
      
      lightStatus[light] = true; // Number Lit Up
      pwm2.setPWM(light, 4096, 0);
    }

    if(light >= 46 && light <= 60){
      lightStatus[79] = true; // G Light Up    
      pwm3.setPWM(0, 4096, 0);
      
      lightStatus[light] = true; // Number Lit Up
      pwm3.setPWM(light, 4096, 0);
    }
    
    if(light >= 61 && light <= 75){
      lightStatus[80] = true; // O Light Up    
      pwm4.setPWM(0, 4096, 0);
      
      lightStatus[light] = true; // Number Lit Up
      pwm4.setPWM(light, 4096, 0);
    }


    /*
      B - 76
      I - 77
      N - 78
      G - 79
      O - 80
    */


  }
}

void loop() {
  ScreenSaver();
 }