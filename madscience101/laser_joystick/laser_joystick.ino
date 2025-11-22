/* Laser Joystick Minimal Example
 by unicornmafia <https://github.com/unicornmafia>
 This code is in the public domain.

Receive joystick signals through ESP Now and 
move servos using joystick values.

 modified 21 Nov 2025
 by Thomas Marsh
*/

#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <ESP32Servo.h>
#include <math.h>

// define pins for digital io
#define SERVO_Y_SIGNAL_PIN      2 // servoY's signal pin is at pin #1
#define SERVO_X_SIGNAL_PIN      1 // servoX's signal pin is at #2
#define LASER_POWER_PIN         3 // laser power is at pin #3

// variables
Servo servoX;  // create servo objects to control servos
Servo servoY;  // create servo objects to control servos

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
    int x;
    int y;
    bool button;
} struct_message;
struct_message myData;

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  int x = map(myData.x, 0, 4000, 180, 0);
  int y = map(myData.y, 0, 4000, 0, 180);
  servoX.write(x);
  servoY.write(y);  
}

void setup() {
  Serial.begin(9600);
  Serial.println("Remote Control Laser Tower Starting up...");
  servoX.attach(SERVO_X_SIGNAL_PIN, 0, 4000); // attaches the servo on pin 18 to the servo object
  servoY.attach(SERVO_Y_SIGNAL_PIN, 0, 4000); // attaches the servo on pin 18 to the servo object
  pinMode(LASER_POWER_PIN, OUTPUT);  
  digitalWrite(LASER_POWER_PIN, HIGH);  // start it on
  // Init ESP-NOW
  WiFi.mode(WIFI_STA);
  esp_now_init();
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}

void loop() {
}
