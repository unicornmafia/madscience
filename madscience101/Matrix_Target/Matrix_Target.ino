/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-piezo-buzzer
 */

#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>
#include "pitches.h"
#include <Adafruit_Protomatter.h>
#define BUZZZER_PIN  11 // ESP32 pin GPIO18 connected to piezo buzzer


uint8_t rgbPins[]  = {42, 41, 40, 38, 39, 37};
uint8_t addrPins[] = {45, 36, 48, 35, 21};
uint8_t clockPin   = 2;
uint8_t latchPin   = 47;
uint8_t oePin      = 14;


Adafruit_Protomatter matrix(
  64,          // Width of matrix (or matrix chain) in pixels
  4,           // Bit depth, 1-6
  1, rgbPins,  // # of matrix chains, array of 6 RGB pins for each
  5, addrPins, // # of address pins (height is inferred), array of pins
  clockPin, latchPin, oePin, // Other matrix control pins
  false);      // No double-buffering here (see "doublebuffer" example)


int melody[] = {
  NOTE_C4, NOTE_G3
};

int noteDurations[] = {
  8, 8
};

unsigned long previousMillis = 0;  // will store last time LED was updated

// constants won't change:
const long interval = 1000;  // interval at which to blink (milliseconds)
unsigned long numMessagesReceived = 0;
unsigned long messagesReceivedInLastSecond = 0;
bool bPlaying = false;

// Structure example to receive data
// Must match the sender structure
typedef struct struct_message {
    int button;
} struct_message;

// Create a struct_message called myData
struct_message myData;


 
void play(int numReps) {
  uint8_t colors[3][3] = {{0,0,255}, {0,255,0}, {255,0,0}}; 
  static int color = 0;

  size_t numNotes = sizeof(melody) / sizeof(melody[0]);
  for (int i=0;i<numReps;i++){
    color = i % 3;
    for (int r=0;r<50;r++){
      matrix.drawCircle(32, 32, r, matrix.color565(colors[color][0], colors[color][1], colors[color][2]));
      matrix.show(); // Copy data to matrix buffers
    }
    
    for (int thisNote = 0; thisNote < numNotes; thisNote++) {
      int noteDuration = 1000 / noteDurations[thisNote];
      tone(BUZZZER_PIN, melody[thisNote], noteDuration);

      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
      noTone(BUZZZER_PIN);
      bPlaying=false;
    }
    
  }
  matrix.fillScreen(0);
  matrix.show(); // Copy data to matrix buffers

}


// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  /*Serial.print("Bytes received: ");
  Serial.println(len);

  Serial.print("x: ");
  Serial.println(myData.x);
  Serial.print("y: ");
  Serial.println(myData.y);
  Serial.print("Button: ");
  Serial.println(myData.button);
  Serial.println();*/
  numMessagesReceived ++;
  messagesReceivedInLastSecond ++;
  if (myData.button == 1){
    bPlaying=true;
    play(3);
  }
}
 
void readMacAddress(){
  uint8_t baseMac[6];
  esp_err_t ret = esp_wifi_get_mac(WIFI_IF_STA, baseMac);
  if (ret == ESP_OK) {
    Serial.printf("%02x:%02x:%02x:%02x:%02x:%02x\n",
                  baseMac[0], baseMac[1], baseMac[2],
                  baseMac[3], baseMac[4], baseMac[5]);
  } else {
    Serial.println("Failed to read MAC address");
  }
}


void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Initialize matrix...
  ProtomatterStatus status = matrix.begin();
  Serial.print("Protomatter begin() status: ");
  Serial.println((int)status);
  if(status != PROTOMATTER_OK) {
    // DO NOT CONTINUE if matrix setup encountered an error.
    for(;;);
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
  delay(1000);
  readMacAddress();
  Serial.println("Starting Matrix");
  play(3);
  
}


void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    //Serial.print("ESPNOW ESP32 Board Listening on MAC Address: ");
    //Serial.print("Total Messages Received: ");
    //Serial.print(numMessagesReceived);
    //Serial.print(",   Messages/second: ");
    //Serial.print(",");
    Serial.print(messagesReceivedInLastSecond);
    Serial.print(",");
    // Serial.print(myData.x);
    // Serial.print(",");
    // Serial.print(myData.y);
    // Serial.print(",");
    Serial.println(myData.button);
    messagesReceivedInLastSecond = 0;

  }
  delay(1);
 
}