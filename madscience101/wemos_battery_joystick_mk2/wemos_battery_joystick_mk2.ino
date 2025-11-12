/*
  Thomas Marsh
*/
#include <esp_now.h>
#include <WiFi.h>

// REPLACE WITH YOUR RECEIVER MAC Address
// char mac[]="8c:d0:b2:a9:38:31";
// uint8_t broadcastAddress[] = {0x8C, 0xD0, 0xB2, 0xA9, 0x38, 0x31};

char mac[]="20:6e:f1:6d:1b:14";
uint8_t broadcastAddress[] = {0x20, 0x6e, 0xf1, 0x6d, 0x1b, 0x14};

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  int x;
  int y;
  bool button;
} struct_message;

// Create a struct_message called myData
struct_message myData;

unsigned long previousMillis = 0;  // will store last time LED was updated

// constants won't change:
const long interval = 1000;  // interval at which to blink (milliseconds)
unsigned long messagesSent = 0;
unsigned long messagesSentInLastSecond = 0;

esp_now_peer_info_t peerInfo;
bool bSentCenter = false;

#define VRX_PIN  39 // ESP32 pin GPIO39 (ADC3) connected to VRX pin
#define VRY_PIN  36 // ESP32 pin GPIO36 (ADC0) connected to VRY pin
#define SW_PIN  17
#define LED_PIN  16

//ezButton button(SW_PIN);

int valueX = 0; // to store the X-axis value
int valueY = 0; // to store the Y-axis value
int bValue = 0; // To store value of the button



// callback when data is sent

void OnDataSent(const esp_now_send_info_t *tx_info, esp_now_send_status_t status) {
  //Serial.print("\r\nLast Packet Send Status:\t");
  //Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  messagesSent++;
  messagesSentInLastSecond++;
}
 
void setup() {
  // Init Serial Monitor
  Serial.begin(115200);

  // define LED pin
  pinMode(LED_PIN, OUTPUT);

  // define button for joystick switch 
  pinMode(SW_PIN, INPUT_PULLUP);  // pullup because this is default high 
                                  // pushing the button makes it go low

  // set analog read attenuation
  analogSetAttenuation(ADC_11db);

  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  
  // Register peer
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}
 
void loop() {

  // get values from hardware
  bValue = !digitalRead(SW_PIN);  // we are using ! because by default when pressed, it goes low.
  valueX = analogRead(VRX_PIN); 
  valueY = analogRead(VRY_PIN);

  // check to see if there's "real" data to send 
  //     some randomness is to be expected.  we are filtering it out
  if (bValue || valueX > 1900 || valueX < 1800
      || valueY > 1900 || valueY < 1800)
  {
    // Set values to send
    myData.x = valueX; 
    myData.y = valueY;   
    myData.button = bValue;
    // Send message via ESP-NOW
    bSentCenter=false;
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
  } else if (!bSentCenter) {
    // Set values to send
    myData.x = 1850; 
    myData.y = 1850;   
    myData.button = 0;
    // Send message via ESP-NOW
    bSentCenter=true;
    esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
    Serial.println("Sending Center");
  }
  
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;
    Serial.print("Messages/Second Sent: ");
    Serial.println(messagesSentInLastSecond);
    
    messagesSentInLastSecond = 0;
    
  }
  delay(10);

}