
#define LASER_POWER_PIN         3 // laser power is at pin #3


void setup() {
  pinMode(LASER_POWER_PIN, OUTPUT);  
  digitalWrite(LASER_POWER_PIN, HIGH);  // start it on
}

void loop() {
  digitalWrite(LASER_POWER_PIN, HIGH);  // turn it on
  delay(500);
  digitalWrite(LASER_POWER_PIN, LOW);  // turn it off
  delay(500);
}
