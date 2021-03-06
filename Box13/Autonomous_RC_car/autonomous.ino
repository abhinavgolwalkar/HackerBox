/**************************************************************
 * This example runs directly on ESP8266 chip
 * using the Blynk platform and mobile application.
 * In the Blynk App you control the car in following modes:
 *  - Manual
 *  - Autonomous
 *  
 * Change Motor connection pin ,WiFi ssid, password, 
 * and Blynk auth token to run :)
 **************************************************************/

#define BLYNK_PRINT Serial    // Comment this out to disable prints and save space
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SimpleTimer.h>

#define RightMotorSpeed 5
#define RightMotorDir   0
#define LeftMotorSpeed  4
#define LeftMotorDir    2
#define echoPin 13 // Echo Pin
#define trigPin 12 // Trigger Pin
ADC_MODE(ADC_VCC);

// You should get Auth Token in the Blynk App.
// Go to the Project Settings (nut icon).
char auth[] = "**********";
char ssid[] = "**********";
char pass[] = "**********";

int maximumRange = 200; // Maximum range needed
int minimumRange = 0; // Minimum range needed
long duration, distance; // Duration used to calculate distance
float Input_Voltage; //To store Battery Voltage
int RC_mode = 0;  //0 - manual ; 1 - auto
int dir_to_turn = 0; //0 - Left; 1- Right

SimpleTimer timer;
void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);

  pinMode(RightMotorSpeed, OUTPUT);
  pinMode(RightMotorDir, OUTPUT);
  pinMode(LeftMotorSpeed, OUTPUT);
  pinMode(LeftMotorDir, OUTPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  //randomSeed(1);
}

void loop()
{
  Blynk.run();
  if(RC_mode) {
    //Serial.print("In Auto mode\n");    
    if ( distance > 20 ) {
      //Serial.print("  Forward\n");
      Forward();

      /* Determine direction to trurn if 
      if obstacle is encountered */
      int rand_number = random(10);
      if (rand_number >=5 ) {
        dir_to_turn = 1;
      } else {
        dir_to_turn = 0;
      }
    } else {
      if (dir_to_turn == 1 ) {
        //Serial.print("  Right\n");
        Right();
      } else {
        //Serial.print("  Left\n");
        Left();
      }
      delay(200);
      halt();
      delay(100);
    }
  } else {
    //Serial.print("In Manual Mode\n");
  }
  //delay(500);
}

BLYNK_WRITE(V6) {
  RC_mode = param.asInt();
  halt();
}

BLYNK_READ(V5) {
  Input_Voltage = analogRead(A0) / 10000;
  Blynk.virtualWrite(V5,Input_Voltage);
}

BLYNK_READ(V4) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH);
  distance = duration/58.2;
  if (distance >= maximumRange || distance <= minimumRange){
    Blynk.virtualWrite(V4,distance);
  } else {
    Blynk.virtualWrite(V4,distance);
  }  
}

void halt() {
  digitalWrite(RightMotorSpeed, LOW);
  digitalWrite(LeftMotorSpeed, LOW);
}

void Right() {
  digitalWrite(RightMotorDir, HIGH);
  digitalWrite(LeftMotorDir, HIGH);
  digitalWrite(RightMotorSpeed, HIGH);
  digitalWrite(LeftMotorSpeed, HIGH);
}

void Left() {
  digitalWrite(RightMotorDir, LOW);
  digitalWrite(LeftMotorDir, LOW);
  digitalWrite(RightMotorSpeed, HIGH);
  digitalWrite(LeftMotorSpeed, HIGH);
}

void Reverse() {
  digitalWrite(RightMotorDir, LOW);
  digitalWrite(LeftMotorDir, HIGH);
  digitalWrite(RightMotorSpeed, HIGH);
  digitalWrite(LeftMotorSpeed, HIGH);
}

void Forward() {
  digitalWrite(RightMotorDir, HIGH);
  digitalWrite(LeftMotorDir, LOW);
  digitalWrite(RightMotorSpeed, HIGH);
  digitalWrite(LeftMotorSpeed, HIGH);
}

BLYNK_WRITE(V0) {
  if (param[0] && !RC_mode)
    Right();
  else
    halt();
}

BLYNK_WRITE(V1) {
  if (param[0] && !RC_mode)
    Left();
  else
    halt();
}

BLYNK_WRITE(V2) {
  if (param[0] && !RC_mode)
    Reverse();
  else
    halt();
}

BLYNK_WRITE(V3) {
  if (param[0] && !RC_mode)
    Forward();
  else
    halt();
}

BLYNK_CONNECTED() {
  Blynk.syncVirtual(V6);
}
