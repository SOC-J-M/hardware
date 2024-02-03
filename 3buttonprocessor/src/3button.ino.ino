int counterButtonPin = 3; // Button to increment the counter
int errorButtonPin = 5;   // Button to print "ERROR"
int errorFreeButtonPin = 6; // Button to print "ERROR FREE"
int buttonPressCounter = 0; // Counter for the number of button presses

void setup() {
  Serial.begin(9600);
  // Initialize pins as input with internal pull-up resistor
  pinMode(counterButtonPin, INPUT_PULLUP);
  pinMode(errorButtonPin, INPUT_PULLUP);
  pinMode(errorFreeButtonPin, INPUT_PULLUP);
}

void loop() {
  // Button for counter
  if (digitalRead(counterButtonPin) == LOW) {
    // Increment counter and print
    buttonPressCounter++;
    Serial.print("Button presses: ");
    Serial.println(buttonPressCounter);
    delay(200); // Debounce delay
  }

  // Button for "ERROR"
  if (digitalRead(errorButtonPin) == LOW) {
    Serial.println("ERROR");
    delay(200); // Debounce delay
  }

  // Button for "ERROR FREE"
  if (digitalRead(errorFreeButtonPin) == LOW) {
    Serial.println("ERROR FREE");
    delay(200); // Debounce delay
  }
}

