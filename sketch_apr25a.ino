#include <WiFiS3.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

String work_order = "Awaiting order...";

// LCD initialization
LiquidCrystal_I2C lcd(0x27, 20, 4);

// Initialize a constant string for display
const String statusLCD = "Status: Operational";
//const String workOrderStatus = "Work Order #:";

unsigned long startTime;


const char ssid[] = "SpectrumSetup-7D";
const char pass[] = "manygate969"; 

WiFiClient client;
int status = WL_IDLE_STATUS;

int counterButtonPin = 5; // Button to increment the counter
int buttonPressCounter = 0;
unsigned long previous_time = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial1.begin(9600);
  delay(1000);
  //Serial.println(ssid);
  //Serial.println(pass);

  lcd.init();         // Initialize the LCD
  lcd.backlight();    // Turn on the backlight
  updateLCD(statusLCD);  // Update display with initial status

    
  startTime = millis(); // Initialize start time for the timer

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);
    if (status == 3){
      Serial.println("Successfully Connected");
    }
    // wait 10 seconds for connection:
    delay(500);
  }

  pinMode(counterButtonPin, INPUT_PULLUP); //set counter pin

  //waiting for work order to be scanned
  while (work_order == "Awaiting order...") {
    scanOrder();
    delay(1000);
  }
  updateLCD(statusLCD);
}

void sendRequest() {
  if(WiFi.status() == WL_CONNECTED) {
    if (client.connect("192.168.1.178", 5001)) {
      client.println("POST /dataQuery?quantity=" + String(buttonPressCounter) + "&work_order=" + String(work_order) + " HTTP/1.1");
      client.println("Host: 10.0.0.59");
      client.println("Connection: close");
      client.println();
      buttonPressCounter = 0;
      while (client.connected()) {
        if (client.available()) {
          Serial.write(client.read());
        }
      }
      client.stop();
    } else {
      Serial.println("Failed to connect to server.");
    }
  } else {
    Serial.println("WiFi not connected.");
  }
}

void scanOrder(){
  String message = "";
  bool scanned = false;
  while (Serial1.available()) {
    char readChar = Serial1.read();
    message += readChar;
    scanned = true;
    delay(10);
  }
  if (scanned) {
    work_order = message;
    updateLCD(statusLCD);
    //Serial.print(work_order);
    //Serial.println();
  }
}

void updateLCD(const String& statusMessage) {
  lcd.clear(); // Clear the display
  lcd.setCursor(0, 0);
  lcd.print(statusMessage); // Print the status message on the first line

  lcd.setCursor(0, 1);
  lcd.print("Timer: ");
  
  lcd.setCursor(0, 2);
  lcd.print(work_order); // Print the work order status on the third line
}

void loop() {
  unsigned long currentTime = millis();
  unsigned long elapsedTime = (currentTime - startTime) / 1000; // Calculate elapsed time in seconds
  
  // Update the timer display every second
  lcd.setCursor(7, 1);
  lcd.print(elapsedTime);
  
  if (Serial1.available()) {
    scanOrder();
  }
  unsigned long cycleMillis = millis() - previous_time;
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
  

  if (cycleMillis >= 5000){
    previous_time = millis();
    //Serial.println("Time: ");
    //Serial.println(currentMillis);
    //Serial.println("Button after time: ");
    Serial.println("");
    Serial.print(buttonPressCounter);
    Serial.print(",");
    Serial.print("WorkOrder");
    Serial.println(work_order);
    sendRequest();
    
  }
}
