int counterButtonPin = 3; // Button to increment the counter
int errorButtonPin = 5;   // Button to print "ERROR"
int errorFreeButtonPin = 6; // Button to print "ERROR FREE"
int buttonPressCounter = 0; // Counter for the number of button presses
unsigned long previous_time = 0;

void setup() {
  Serial.begin(9600);
  // Initialize pins as input with internal pull-up resistor
  pinMode(counterButtonPin, INPUT_PULLUP);
  pinMode(errorButtonPin, INPUT_PULLUP);
  pinMode(errorFreeButtonPin, INPUT_PULLUP);
}

void loop() {

  unsigned long currentMillis = millis() - previous_time;
  // Button for counter
  if (digitalRead(counterButtonPin) == HIGH) {
    // Increment counter and print
    delay(10);
    if (digitalRead(counterButtonPin) == LOW){
      buttonPressCounter++;
      //Serial.print("Button presses: ");
      //Serial.println(buttonPressCounter);
      delay(200); // Debounce delay
    }
  }
  // delay(1000);
  // buttonPressCounter++;
  

  if (currentMillis >= 5000){
    previous_time = millis();
    //Serial.println("Time: ");
    //Serial.println(currentMillis);
    //Serial.println("Button after time: ");
    Serial.print(buttonPressCounter);
    Serial.print(",");
    Serial.println("WorkOrder");
    buttonPressCounter = 0;
  }

  

  // Button for "ERROR"
  // if (digitalRead(errorButtonPin) == LOW) {
  //   Serial.println("ERROR");
  //   delay(200); // Debounce delay
  // }

  // // Button for "ERROR FREE"
  // if (digitalRead(errorFreeButtonPin) == LOW) {
  //   Serial.println("ERROR FREE");
  //   delay(200); // Debounce delay
  // }
}

