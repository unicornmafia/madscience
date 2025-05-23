/* CatLaser MK IV
 by unicornmafia <https://github.com/unicornmafia>
 This code is in the public domain.

 modified 20 Jan 2024
 by Thomas Marsh
*/


#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include <ESP32Servo.h>
#include <math.h>

// define pins for digital io
#define ESP_C3_LED_BUILTIN_PIN  8 // Builtin LED (blue) on front.  Also outputs to digital io pin #8
#define SERVO_Y_SIGNAL_PIN      2 // servoY's signal pin is at pin #1
#define SERVO_X_SIGNAL_PIN      1 // servoX's signal pin is at #2
#define LASER_POWER_PIN         3 // laser power is at pin #3


// set the max angles of the servo turret to 45 degrees
#define SERVO_MIN_ANGLE_X 0 
#define SERVO_MAX_ANGLE_X 180
#define SERVO_MIN_ANGLE_Y 0 
#define SERVO_MAX_ANGLE_Y 180
#define SERVO_REVERSE_X 1
#define SERVO_REVERSE_Y 0
#define SERVO_RANGE_X SERVO_MAX_ANGLE_X - SERVO_MIN_ANGLE_X
#define SERVO_RANGE_Y SERVO_MAX_ANGLE_Y - SERVO_MIN_ANGLE_Y


// variables
Servo servoX;  // create servo objects to control servos
Servo servoY;  // create servo objects to control servos

int xPos = SERVO_MIN_ANGLE_X;
int yPos = SERVO_MIN_ANGLE_Y;


int x;
int y;
bool b;

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
    int x;
    int y;
    bool button;
} struct_message;

// Create a struct_message called myData
struct_message myData;




void readMacAddress(){
  uint8_t baseMac[6];
  esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
  if (ret == ESP_OK) {
    Serial.printf("mac: %02x:%02x:%02x:%02x:%02x:%02x, x: %d, y: %d, b: %d\n",
                  baseMac[0], baseMac[1], baseMac[2],
                  baseMac[3], baseMac[4], baseMac[5],
                  x,y,b);
  } else {
    Serial.println("Failed to read MAC address");
  }
}


void setLaserPosition(int xPos, int yPos){

  if (SERVO_REVERSE_X){
    // remap backwards
    xPos = map(xPos, SERVO_MIN_ANGLE_X, SERVO_MAX_ANGLE_X,SERVO_MAX_ANGLE_X,SERVO_MIN_ANGLE_X);
  }
  if (SERVO_REVERSE_Y){
    // remap backwards
    yPos = map(yPos, SERVO_MIN_ANGLE_Y, SERVO_MAX_ANGLE_Y,SERVO_MAX_ANGLE_Y,SERVO_MIN_ANGLE_Y);
  }
  servoX.write(xPos);
  servoY.write(yPos);  
}

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  // Serial.print("Bytes received: ");
  // Serial.println(len);

  // Serial.print("x: ");
  // Serial.println(myData.x);
  // Serial.print("y: ");
  // Serial.println(myData.y);
  // Serial.print("Button: ");
  // Serial.println(myData.button);
  // Serial.println();

  

  int xPos = map(myData.x, 0, 4000, SERVO_MIN_ANGLE_X, SERVO_MAX_ANGLE_X);
  int yPos = map(myData.y, 0, 4000, SERVO_MIN_ANGLE_Y, SERVO_MAX_ANGLE_Y);
  x = xPos;
  y = yPos;
  b = myData.button;
  setLaserPosition(xPos, yPos);
}

void setupServos() {
  // specific stuff for esp32-c3 servo control
  // Allow allocation of all timers
	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	servoX.setPeriodHertz(50);    // standard 50 hz servo
  servoY.setPeriodHertz(50);    // standard 50 hz servo
	servoX.attach(SERVO_X_SIGNAL_PIN, 1000, 2000); // attaches the servo on pin 18 to the servo object
  servoY.attach(SERVO_Y_SIGNAL_PIN, 1000, 2000); // attaches the servo on pin 18 to the servo object
	// using default min/max of 1000us and 2000us
	// different servos may require different min/max settings
	// for an accurate 0 to 180 sweep

  delay(100); // delay for 100 ms 
  setLaserPosition(xPos, yPos);
}

void setupLaserPower(){
  pinMode(LASER_POWER_PIN, OUTPUT);  
  digitalWrite(LASER_POWER_PIN, HIGH);  // start it on
}

void setupBlink() {
  pinMode(ESP_C3_LED_BUILTIN_PIN, OUTPUT);  
  digitalWrite(ESP_C3_LED_BUILTIN_PIN, HIGH);  // start it on
}






void drawSquare() {
  Serial.println("Drawing Square");
  // loop x and y servos from min to max.  should make a square with the laser

  int slowdown = 2;
  // start lower left
  xPos = SERVO_MIN_ANGLE_X;
  yPos = SERVO_MIN_ANGLE_Y;
  setLaserPosition(xPos, yPos);

  // bottom left to bottom right
  for (int pos=0; pos < 180; pos++ ) {
    // this converts the value in the range 0->180 to the value ranges defined by the servo angles
    xPos = map(pos, 0, 180, SERVO_MIN_ANGLE_X, SERVO_MAX_ANGLE_X);
    setLaserPosition(xPos, yPos);
    delay(slowdown);
  }
  xPos = SERVO_MAX_ANGLE_X;
  setLaserPosition(xPos, yPos);

  // bottom right to top right
  for (int pos=0; pos < 180; pos++ ) {
    // this converts the value in the range 0->180 to the value ranges defined by the servo angles
    yPos = map(pos, 0, 180, SERVO_MIN_ANGLE_Y, SERVO_MAX_ANGLE_Y);
    setLaserPosition(xPos, yPos);
    delay(slowdown);
  }
  yPos = SERVO_MAX_ANGLE_Y;
  setLaserPosition(xPos, yPos);

  // top right to top left
  for (int pos=180; pos > 0; pos-- ) {
    // this converts the value in the range 0->180 to the value ranges defined by the servo angles
    xPos = map(pos, 0, 180, SERVO_MIN_ANGLE_X, SERVO_MAX_ANGLE_X);
    setLaserPosition(xPos, yPos);
    delay(slowdown);
  }
  xPos = SERVO_MIN_ANGLE_X;
  setLaserPosition(xPos, yPos);

  // top left to lower left
  for (int pos=180; pos > 0; pos-- ) {
    // this converts the value in the range 0->180 to the value ranges defined by the servo angles
    yPos = map(pos, 0, 180, SERVO_MIN_ANGLE_Y, SERVO_MAX_ANGLE_Y);
    setLaserPosition(xPos, yPos);
    delay(slowdown);
  }
  yPos = SERVO_MIN_ANGLE_Y;
  setLaserPosition(xPos, yPos);
}


//radians = (degrees * 71) / 4068
//degrees = (radians * 4068) / 71

void drawCircle() {
  Serial.println("Drawing Circle");
  // loop x and y servos from min to max.  should make a square with the laser

  int SERVO_MID_ANGLE_X = (int)round(float(SERVO_RANGE_X) / 2.0) + SERVO_MIN_ANGLE_X;
  int SERVO_MID_ANGLE_Y = (int)round(float(SERVO_RANGE_Y) / 2.0) + SERVO_MIN_ANGLE_Y;

  int slowdown = 1;
  int radians_min = 0;
  int radians_max = 180;

  // plot top
  Serial.println("plotting top");

  for (int x=0; x < 180; x++ ) {
    float radians = (x * 71.0) / 4068.0;
    int y = round(sin(radians) * (float)radians_max);
    // this converts the value in the range 0->180 to the value ranges defined by the servo angles

    xPos = map(x, 0, 180, SERVO_MIN_ANGLE_X, SERVO_MAX_ANGLE_X);
    yPos = map(y, radians_min, radians_max, SERVO_MID_ANGLE_Y, SERVO_MAX_ANGLE_Y);
    setLaserPosition(xPos, yPos);
  
    delay(slowdown);
  }


  Serial.println("plotting bottom");
  // plot bottom backwards
  for (int x=180; x > 0; x-- ) {
    float radians = (x * 71.0) / 4068.0;
    int y = round((1.0-sin(radians)) *  (float)radians_max);

    // this converts the value in the range 0->180 to the value ranges defined by the servo angles
    xPos = map(x, 0, 180, SERVO_MIN_ANGLE_X, SERVO_MAX_ANGLE_X);
    yPos = map(y, radians_min, radians_max, SERVO_MIN_ANGLE_Y, SERVO_MID_ANGLE_Y);
    setLaserPosition(xPos, yPos);

    delay(slowdown);
  }

}

void loopServos() {
  drawCircle();
  delay(600);
  drawSquare();
  delay(600);
}

void loopBlink() {
  Serial.println("Blinking LED");
  digitalWrite(ESP_C3_LED_BUILTIN_PIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(500);                      // wait for a second
  digitalWrite(ESP_C3_LED_BUILTIN_PIN, LOW);   // turn the LED off by making the voltage LOW
  delay(500);   
}

void loopMac() {
  //Serial.print("ESPNOW ESP32 Board Listening on MAC Address: ");
  readMacAddress();
}

void laserBlink() {
  Serial.println("Blinking Laser");
  digitalWrite(LASER_POWER_PIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(500);                      // wait for half a second
  digitalWrite(LASER_POWER_PIN, LOW);   // turn the LED off by making the voltage LOW
  delay(500);   
}

void setupEspNow(){
    // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
}

void setup() {
  Serial.begin(9600);
  Serial.println("CatBot MK IV Starting up...");


  setupEspNow();
  setupBlink();
  setupLaserPower();
  setupServos();  
}

void loop() {
  //loopServos();
  loopMac();
  delay(1);
  /* 
  disabling this for now
  loopBlink();
  laserBlink();
  delay(1000);
  */
}
