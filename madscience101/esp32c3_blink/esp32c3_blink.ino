#define ESP_C3_LED_BUILTIN_PIN  8 // Builtin LED (blue) on front.  
                                  // Also outputs to digital io pin #8

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(ESP_C3_LED_BUILTIN_PIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(ESP_C3_LED_BUILTIN_PIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);                      // wait for a second
  digitalWrite(ESP_C3_LED_BUILTIN_PIN, LOW);   // turn the LED off by making the voltage LOW
  delay(1000);                      // wait for a second
}
