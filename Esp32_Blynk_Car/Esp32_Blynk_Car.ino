#define BLYNK_PRINT Serial

#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>

// L298N pinout
// Motor 1
#define en1 25
#define m1 18
#define m2 19
// Motor 2
#define en2 26
#define m3 22
#define m4 23

// HC-SR04 pinout
#define trigPin 32
#define echoPin 33

// Your blynk Auth Token
char auth[] = "c192d2d8f78b48178ac90dfdb23ded99";

// Your WIFI Credentials
// Set password to "" for open networks
char ssid[] = "ZON-7D20";
char pass[] = "bce392613e1f";

// PWM variable to controll the motor speed
int motorSpeed;

// PWM variables
const int freq = 1000;
const int mChannel = 0;
const int resolution = 8;

// Variables for HC-SR04
unsigned long duration;
float distance;

// If its true the car can't move forward
bool motorBreak = false;

// Checks if the car is moving forward
bool isForward = false;

// This function reads the Virtual Pin (1)
BLYNK_WRITE(V1){
  motorSpeed = param.asInt();
}

// This function reads the joystick's values, Virtual Pin (2)
BLYNK_WRITE(V2) {
  int x = param[0].asInt();
  int y = param[1].asInt();

  if((x > 256 && x < 768) && (y > 256 && y < 768)){
    mot_stop();
    isForward = false;
  }
  else if(y < 256 && (x > 256 && x < 768)){
    mot_down();
    isForward = false;
  }
  else if((y > 768 && (x > 256 && x < 768)) && !motorBreak){
    mot_up();
    isForward = true;
  }
  else if(x < 256 && (y > 256 && y < 768)){
    mot_left();
    isForward = false;
  }
  else if(x > 768 && (y > 256 && y < 768)){
    mot_right();
    isForward = false;
  }
}

void mot_right() {
   digitalWrite(m1, 1); // turn on 
   digitalWrite(m3, 1); // turn on 
   digitalWrite(m2, 0); // turn off
   digitalWrite(m4, 0); // turn off
}

void mot_left() {
   digitalWrite(m2, 1); // turn on 
   digitalWrite(m4, 1); // turn on 
   digitalWrite(m1, 0); // turn off
   digitalWrite(m3, 0); // turn off
}

void mot_stop(){
   digitalWrite(m2, 0); // turn off
   digitalWrite(m3, 0); // turn off
   digitalWrite(m1, 0); // turn off
   digitalWrite(m4, 0); // turn off
}

void mot_up(){
  digitalWrite(m2, 1); // turn on 
  digitalWrite(m3, 1); // turn on 
  digitalWrite(m1, 0); // turn off
  digitalWrite(m4, 0); // turn off
}

void mot_down(){
  Serial.println("left");
  digitalWrite(m1, 1); // turn on 
  digitalWrite(m4, 1); // turn on 
  digitalWrite(m2, 0); // turn off
  digitalWrite(m3, 0); // turn off
}

void setup()
{
  Serial.begin(115200); 
  while (!Serial);

  // Setup the HC-SR04 trig pin and echo pin respectively
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);

  // Setup the en1 and en2 pins with PWM
  ledcSetup(mChannel, freq, resolution);
  ledcAttachPin(en1, mChannel);
  ledcAttachPin(en2, mChannel);

  // Initial value for the PWM (0-255)
  ledcWrite(mChannel, 255);

  // Setup the pins for the motors
  pinMode(m1, OUTPUT);
  pinMode(m2, OUTPUT);
  pinMode(m3, OUTPUT);
  pinMode(m4, OUTPUT); 

  // Connect Esp32 to blynk server
  Blynk.begin(auth, ssid, pass);
}

void loop()
{
  Blynk.run();

  // calculate the distance
  digitalWrite(trigPin,HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration / 58.31;

  // If the distance is lower than 20cm then the motors will stop;
  if(distance < 20 && isForward){
    mot_stop();
    motorBreak = true;
  }
  else{
    motorBreak = false;
  }

  // Sets the motors speed
  ledcWrite(mChannel, motorSpeed);  
}
