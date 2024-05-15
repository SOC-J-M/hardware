const int inputPin = 4;

void setup() {
  // Set the pin as an input
  pinMode(inputPin, INPUT);

  // Initialize serial communication at 9600 bits per second:
  Serial.begin(9600);
}

void loop() {
  // Read the state of the input pin
  int pinState = digitalRead(inputPin);

  // Print out the state of the pin
  if(pinState == HIGH) {
    Serial.println("3.3V source is ON");
  } else {
    Serial.println("3.3V source is OFF");
  }

  // Wait for a second before checking again
  delay(1000);
}
