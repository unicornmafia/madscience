/*
  Thomas Marsh
*/
#include <esp_now.h>
#include <WiFi.h>

#define VRX_PIN  39 // ESP32 pin GPIO39 (ADC3) connected to VRX pin
#define VRY_PIN  36 // ESP32 pin GPIO36 (ADC0) connected to VRY pin
#define SW_PIN  17
#define LED_PIN  16

uint8_t thomasDemoMAC[] = {0x20, 0x6e, 0xf1, 0x6d, 0x1b, 0x14};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  int x;
  int y;
  bool button;
} struct_message;

// Create a struct_message called myData
struct_message myData;

esp_now_peer_info_t peerInfo;
bool bSentCenter = false;


void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  // define LED pin
  pinMode(LED_PIN, OUTPUT);
  // define button for joystick switch 
  pinMode(SW_PIN, INPUT_PULLUP);  // pullup because this is default high 
                                  // pushing the button makes it go low
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  // Init ESP-NOW
  esp_now_init();
  // Register peer
  memcpy(peerInfo.peer_addr, thomasDemoMAC, sizeof(thomasDemoMAC));
  // Add peer        
  esp_now_add_peer(&peerInfo);
}
 
void loop() {
  // get values from hardware
  myData.button = !digitalRead(SW_PIN);  // we are using ! because by default when pressed, it goes low.
  myData.x = analogRead(VRX_PIN); 
  myData.y = analogRead(VRY_PIN);

  esp_err_t result = esp_now_send(thomasDemoMAC, (uint8_t *) &myData, sizeof(myData));
  delay(10);
}