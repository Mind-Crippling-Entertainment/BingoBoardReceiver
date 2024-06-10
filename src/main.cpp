#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <string.h>
#include <espnow.h>
#include <Adafruit_PWMServoDriver.h>

/**
 * This firmware is for an ESP82266 
 * Coded is Open Under GNU V3.
 * This requires matching hardware configurations. 
 */

/**
 * This firmware version will show on the LCD.
 * It is suggested keeping the firmware version for the Sender the same as the Receiver
 */
const String version = "0.0.1"; //This is like Alpha Level Code ATM.

//Enable or Disable Debugging
#define DEBUG = true;

//Set the address and line count for the LCD Display
LiquidCrystal_I2C lcd(0x27,20,4);

//This is the duration that the LCD screen will show whatever is displayed.
const int screenOnDuration = 4000;
//Setup the Screen Savers counter. 
//(This tracks how long the screen has been on.)
unsigned long screenSaverRunTime = 0;


// Here we initialise each function that we are going to use in our code.
void lcdDisplay(String, String, String, String);
void ScreenSaver(void);
void ToggleLight(int);
void ProcessInput(char);


/**
 * Setting up each PWM Driver for each letter.
 * Each Bingo letter has the option of 15 numbers
 * If we light up the Letter this makes 16 channels per Bingo Letter.
 * This makes one Module perfect for each letter.
 */


Adafruit_PWMServoDriver pwm0 = Adafruit_PWMServoDriver(0x41); // Pins 0-15 B
Adafruit_PWMServoDriver pwm1 = Adafruit_PWMServoDriver(0x42); // Pins 0-15 I
Adafruit_PWMServoDriver pwm2 = Adafruit_PWMServoDriver(0x43); // Pins 0-15 N
Adafruit_PWMServoDriver pwm3 = Adafruit_PWMServoDriver(0x44); // Pins 0-15 G
Adafruit_PWMServoDriver pwm4 = Adafruit_PWMServoDriver(0x45); // Pins 0-15 O


// Must match the sender structure
typedef struct struct_message {
    //We only need the information coming from keypad.
    char a;
    
} struct_message;

// Create a struct_message called myData
struct_message myData;



/**
 * This function is part of ESPNOW.
 * This is triggered automatically when any data is recieved for This Device on the ESPNow system.
 */
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&myData, incomingData, sizeof(myData));
  lcdDisplay("Bytes received: " + len, "Bingo Number: "  + String(myData.a), "", "" );
  ProcessInput(myData.a);
} 

//Define our variables.

//Create array for if the LED Light is On or Off
bool lightStatus[81]; 

void setup()
{
  // Initialize the I2C interface.
  Wire.begin(D1,D2);
  // Initialize the LCD.
  lcd.init();
  
  //create a temporary int for looping though and setting all the lights to off in the array.
  int i = 0;

//Loop through all the light numbers in the array and set the value to false to indicate off.
  while(i <= 80){
    lightStatus[i] = false;
    i++;
  }

/**
 * Each PWM MOD needs to be configured.
 * Because we are using this in more of an analog mode, the speed not be set as for with servos.
 * So we just CRANK IT UP ;)
 * 
 */

  pwm0.begin();
  pwm0.setOscillatorFrequency(27000000);
  pwm0.setPWMFreq(1600); 

  pwm1.begin();
  pwm1.setOscillatorFrequency(27000000);
  pwm1.setPWMFreq(1600);  

  pwm2.begin();
  pwm2.setOscillatorFrequency(27000000);
  pwm2.setPWMFreq(1600);  

  pwm3.begin();
  pwm3.setOscillatorFrequency(27000000);
  pwm3.setPWMFreq(1600); 

  pwm4.begin();
  pwm4.setOscillatorFrequency(27000000);
  pwm4.setPWMFreq(1600);

#ifdef DEBUG

  Serial.begin(115200);
  Serial.println("Connected");
  Serial.println();
  Serial.print("ESP8266 Board MAC Address:  ");
  Serial.println(WiFi.macAddress());
 
#endif
  
  lcdDisplay(
    "Bingo Display Board",
    "_-_-_-_-_-_-_-_-_-_",
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
    
    #ifdef DEBUG
    Serial.println("Error initializing ESP-NOW");
    #endif

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

      case 'B':
      break;

      case 'C':
      break;

      case 'D':
      break;

      case '#':
      break;
 
      case '*':
      break;

      default:
      ToggleLight(incomingData);
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



/**
 * This per se is the brains of the entire operation.
 * First we make sure then numbers are in the proper range.
 * Then we figure out light status, and toggle it accordingly.
 * 
 * The only thing we pass to this function is the int of the light itself.
 */

void ToggleLight(int light){

  /**
   * 
   * Each space in the 'B' column contains a number from 1 - 15.
   * Each space in the 'I' column contains a number from 16 - 30.
   * Each space in the 'N' column contains a number from 31 - 45.
   * Each space in the 'G' column contains a number from 46 - 60.
   * Each space in the 'O' column contains a number from 61 - 75. 
   *  B - 76
   *  I - 77
   *  N - 78
   *  G - 79
   *  O - 80
   */

  //DO NOTHING if the light is not in the proper range.
  if(light >=76){
    //During debug announce int out of range.
    #ifdef DEBUG
      Serial.println("Number out of range: " + light);
      #endif
    return;
  }
  
  int outCount = 0;
  /**
  * This first if is for the numbers 1-15, Everything with the letter B happens in this statement.
* First checking if the Number is between 1 and 15 then checking if the number is already.
  * If the light is on we turn it off; then we loop through the other possible lights in this letter.
  * If they are all of. we turn off the Letter Light. 
  * 
  * Else we know the light is off, so we will turn the light on.
  * Because we are turning on a light, we also need to check if the letter light is on.
  * If it is not on, then we turn it on.
  * 
  * During all of this we can't forget to update the main array of lightStatus to keep everything in sync.
  */
  if(light <= 15){

    //If the light is ON turn it off.
    if(lightStatus[light]){
      
      pwm0.setPWM(light, 0, 4096);
  
    /**
     * Loop though each of the values and check if they are now all off.
     * If they are all off then turn off the B light as well.
     */
      for( int l = 1; l<= 15; l++ ){
        if(!lightStatus[l]){
          outCount++;
        }
      }
      if(outCount >= 15){
        lightStatus[76] = false; //B turned Off    
        pwm0.setPWM(0, 0, 4096);
      }
      //Reset the outCount for the next Letter Loop.
      outCount = 0;
    }//If the Light is not already ON then turn it on and set the matching table value.
    else{
      //Make sure B is light up.
      if(!lightStatus[76]){
        lightStatus[76] = true; //B Light Up
          pwm0.setPWM(0, 4096, 0); // Turn On Relay 0 for B
      }
      lightStatus[light] = true; // Number Lit Up
      pwm0.setPWM(light, 4096, 0);
    }   
  }

   /**
    * This second if is for the numbers 16-30, Everything with the letter B happens in this statement.
    * First checking if the Number is between 16 and 30 then checking if the number is already.
    * If the light is on we turn it off; then we loop through the other possible lights in this letter.
    * If they are all of. we turn off the Letter Light. 
    * 
    * Else we know the light is off, so we will turn the light on.
    * Because we are turning on a light, we also need to check if the letter light is on.
    * If it is not on, then we turn it on.
    * 
    * During all of this we can't forget to update the main array of lightStatus to keep everything in sync.
    */
   if(light >= 16 && light <= 30){

      //If the light is ON turn it off.
      if(lightStatus[light]){
        
        pwm1.setPWM(light, 0, 4096);
    
      /**
       * Loop though each of the values and check if they are now all off.
       * If they are all off then turn off the B light as well.
       */
        for( int l = 16; l<= 30; l++ ){
          if(!lightStatus[l]){
            outCount++;
          }
        }
        if(outCount >= 15){
          lightStatus[77] = false; //B turned Off    
          pwm1.setPWM(0, 0, 4096);
        }
        //Reset the outCount for the next Letter Loop.
        outCount = 0;
      }//If the Light is not already ON then turn it on and set the matching table value.
      else{
        //Make sure I is light up.
        if(!lightStatus[77]){
          lightStatus[77] = true; //Light Up I
           pwm1.setPWM(0, 4096, 0); // Turn On Relay 0 for I
        }
           lightStatus[light] = true; // Number Lit Up
           pwm1.setPWM((light - 15) , 4096, 0);
      } 
   }

   /**
    * This third if is for the numbers 31-45, Everything with the letter B happens in this statement.
    * First checking if the Number is between 31 and 45 then checking if the number is already.
    * If the light is on we turn it off; then we loop through the other possible lights in this letter.
    * If they are all of. we turn off the Letter Light. 
    * 
    * Else we know the light is off, so we will turn the light on.
    * Because we are turning on a light, we also need to check if the letter light is on.
    * If it is not on, then we turn it on.
    * 
    * During all of this we can't forget to update the main array of lightStatus to keep everything in sync.
    */
   if(light >= 31 && light <= 45){

      //If the light is ON turn it off.
      if(lightStatus[light]){
        
        pwm1.setPWM(light, 0, 4096);
    
      /**
       * Loop though each of the values and check if they are now all off.
       * If they are all off then turn off the B light as well.
       */
        for( int l = 31; l<= 45; l++ ){
          if(!lightStatus[l]){
            outCount++;
          }
        }
        if(outCount >= 15){
          lightStatus[78] = false; //I turned Off    
          pwm2.setPWM(0, 0, 4096);
        }
        //Reset the outCount for the next Letter Loop.
        outCount = 0;
      }//If the Light is not already ON then turn it on and set the matching table value.
      else{
        //Make sure I is light up.
        if(!lightStatus[78]){
          lightStatus[78] = true; //Light Up I
           pwm2.setPWM(0, 4096, 0); // Turn On Relay 0 for I
        }
           lightStatus[light] = true; // Number Lit Up
           pwm2.setPWM((light - 30) , 4096, 0);
      } 
   }

   /**
    * This fourth if is for the numbers 46-60, Everything with the letter B happens in this statement.
    * First checking if the Number is between 46 and 60 then checking if the number is already.
    * If the light is on we turn it off; then we loop through the other possible lights in this letter.
    * If they are all of. we turn off the Letter Light. 
    * 
    * Else we know the light is off, so we will turn the light on.
    * Because we are turning on a light, we also need to check if the letter light is on.
    * If it is not on, then we turn it on.
    * 
    * During all of this we can't forget to update the main array of lightStatus to keep everything in sync.
    */
   if(light >= 46 && light <= 60){

      //If the light is ON turn it off.
      if(lightStatus[light]){
        
        pwm1.setPWM(light, 0, 4096);
    
      /**
       * Loop though each of the values and check if they are now all off.
       * If they are all off then turn off the B light as well.
       */
        for( int l = 46; l<= 60; l++ ){
          if(!lightStatus[l]){
            outCount++;
          }
        }
        if(outCount >= 15){
          lightStatus[79] = false; //B turned Off    
          pwm3.setPWM(0, 0, 4096);
        }
        //Reset the outCount for the next Letter Loop.
        outCount = 0;
      }//If the Light is not already ON then turn it on and set the matching table value.
      else{
        //Make sure I is light up.
        if(!lightStatus[79]){
          lightStatus[79] = true; //Light Up I
           pwm3.setPWM(0, 4096, 0); // Turn On Relay 0 for I
        }
           lightStatus[light] = true; // Number Lit Up
           pwm3.setPWM((light - 45) , 4096, 0);
      } 
   }

   /**
    * This fifth if is for the numbers 61-75, Everything with the letter B happens in this statement.
    * First checking if the Number is between 61 and 75 then checking if the number is already.
    * If the light is on we turn it off; then we loop through the other possible lights in this letter.
    * If they are all of. we turn off the Letter Light. 
    * 
    * Else we know the light is off, so we will turn the light on.
    * Because we are turning on a light, we also need to check if the letter light is on.
    * If it is not on, then we turn it on.
    * 
    * During all of this we can't forget to update the main array of lightStatus to keep everything in sync.
    */
   if(light >= 61 && light <= 75){

      //If the light is ON turn it off.
      if(lightStatus[light]){
        
        pwm4.setPWM(light, 0, 4096);
    
      /**
       * Loop though each of the values and check if they are now all off.
       * If they are all off then turn off the B light as well.
       */
        for( int l = 16; l<= 30; l++ ){
          if(!lightStatus[l]){
            outCount++;
          }
        }
        if(outCount >= 15){
          lightStatus[80] = false; //B turned Off    
          pwm4.setPWM(0, 0, 4096);
        }
        //Reset the outCount for the next Letter Loop.
        outCount = 0;
      }//If the Light is not already ON then turn it on and set the matching table value.
      else{
        //Make sure I is light up.
        if(!lightStatus[80]){
          lightStatus[80] = true; //Light Up I
           pwm4.setPWM(0, 4096, 0); // Turn On Relay 0 for I
        }
           lightStatus[light] = true; // Number Lit Up
           pwm4.setPWM((light - 60) , 4096, 0);
      } 
    }
  }


void loop() {
  ScreenSaver();
 }