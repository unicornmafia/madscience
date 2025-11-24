#include <esp_now.h>
#include <WiFi.h>
#define BUTTON_PIN 17 // GPIO17 pin connected to button
#define JOYSTICK_X_PIN 39 // GPIO36 pin connected to x axis of joystick
#define JOYSTICK_Y_PIN 36 // GPIO39 pin connected to y axis of joystick


uint8_t matrixAddress[] = {0x64, 0xE8, 0x33, 0x88, 0xC4, 0xBC};
int button_state = 1;

// Structure to send data
typedef struct struct_message {
  int x;
  int y;
  int button_state;
} struct_message;
struct_message myData;

esp_now_peer_info_t peerInfo;

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
  myData.button_state = !digitalRead(BUTTON_PIN);
  Serial.print("button_state: ");
  Serial.println(button_state);

  myData.x = analogRead(JOYSTICK_X_PIN);
  myData.y = analogRead(JOYSTICK_Y_PIN);
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(matrixAddress, (uint8_t *) &myData, sizeof(myData));
  delay(1);
}
