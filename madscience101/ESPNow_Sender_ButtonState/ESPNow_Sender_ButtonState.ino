#include <esp_now.h>
#include <WiFi.h>

#define BUTTON_PIN 17 // GPIO17 pin connected to button

typedef struct struct_message {
  int button_state;
} struct_message;
struct_message myData;          // message structure

esp_now_peer_info_t peerInfo;   // info on who we're talking to
uint8_t matrixAddress[] = {0x48, 0x27, 0xE2, 0x16, 0xCF, 0x24};  // 48:27:E2:16:CF:24

int button_state = 1;

void setup() {
  // Init Serial Monitor
  Serial.begin(115200);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // register peer MAC Address
  memcpy(peerInfo.peer_addr, matrixAddress, 6);
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {
  // read button state 
  myData.button_state = !digitalRead(BUTTON_PIN); 
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(matrixAddress, (uint8_t *) &myData, sizeof(myData));
  delay(1);
}
