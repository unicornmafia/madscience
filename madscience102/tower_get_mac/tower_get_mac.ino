#include <WiFi.h>`

void setup() {
  Serial.begin(115200); // Start serial communication
  delay(10); // Give the serial port time to initialize
  
  // Set the Wi-Fi mode to station (STA) or Access Point (AP)
  // or both for this example. You may need to set your Wi-Fi mode
  // if WiFi.macAddress() doesn't work.
  WiFi.mode(WIFI_STA); // Or WIFI_AP or WIFI_STA_AP
  
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress()); // Print the MAC address
}

void loop() {
  // Do nothing in the loop
}