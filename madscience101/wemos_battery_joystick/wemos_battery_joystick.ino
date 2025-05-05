/*
 * This ESP32 code is created by esp32io.com
 *
 * This ESP32 code is released in the public domain
 *
 * For more detail (instruction and wiring diagram), visit https://esp32io.com/tutorials/esp32-joystick
 */

#include <ezButton.h>
#include <esp_now.h>
#include <WiFi.h>

// REPLACE WITH YOUR RECEIVER MAC Address
//e4:b0:63:41:e9:a0 (c6)
//uint8_t broadcastAddress[] = {0xE4, 0xB0, 0x63, 0x41, 0xE9, 0xA0};
//28:37:2f:74:19:38 (tower)
uint8_t broadcastAddress[] = {0x28, 0x37, 0x2F, 0x74, 0x19, 0x38};

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




#define VRX_PIN  39 // ESP32 pin GPIO39 (ADC3) connected to VRX pin
#define VRY_PIN  36 // ESP32 pin GPIO36 (ADC0) connected to VRY pin
#define SW_PIN  17
#define LED_PIN  16

ezButton button(SW_PIN);

int valueX = 0; // to store the X-axis value
int valueY = 0; // to store the Y-axis value
int bValue = 0; // To store value of the button


// callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);  // turn the LED on (HIGH is the voltage level)

  // Set the ADC attenuation to 11 dB (up to ~3.3V input)
  analogSetAttenuation(ADC_11db);
  button.setDebounceTime(50); // set debounce time to 50 milliseconds

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

void send_data(int x, int y, bool button){
  // ignore if in deadband
  // if ((x>1800) && (x<2200) &&
  //     (y>1800) && (y<2200) && 
  //     (!button))
  // {
  //   return;
  // }


  // Set values to send
  myData.x = x;
  myData.y = y;
  myData.button = button;
  
  // Send message via ESP-NOW
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData));
   
  if (result == ESP_OK) {
    Serial.println("Sent with success");
  }
  else {
    Serial.println("Error sending the data");
  }
}

void loop() {
  button.loop(); // MUST call the loop() function first

  // read X and Y analog values
  valueX = analogRead(VRX_PIN);
  valueY = analogRead(VRY_PIN);

  // Read the button value
  bValue = button.getState();

  if (button.isPressed()) {
    Serial.println("The button is pressed");
    // TODO do something here
  }

  if (button.isReleased()) {
    Serial.println("The button is released");
    // TODO do something here
  }

  // print data to Serial Monitor on Arduino IDE
  Serial.print("x = ");
  Serial.print(valueX);
  Serial.print(", y = ");
  Serial.println(valueY);
  Serial.print(" : button = ");
  Serial.println(bValue);
  if (!bValue){
    digitalWrite(LED_PIN, HIGH);
  } else {
    digitalWrite(LED_PIN, LOW);
  }
  //digitalWrite(LED_PIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  //delay(100);
  //digitalWrite(LED_PIN, LOW);  // turn the LED on (HIGH is the voltage level)
  delay(1);
  send_data(valueX, valueY, !bValue);

}
