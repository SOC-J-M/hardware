#include <WiFiS3.h>

const char ssid[] = "SpectrumSetup-7D";
const char pass[] = "manygate969"; 

WiFiClient client;
int status = WL_IDLE_STATUS;

int counterButtonPin = 3; // Button to increment the counter
int errorButtonPin = 5;   // Button to print "ERROR"
int errorFreeButtonPin = 6; // Button to print "ERROR FREE"
int buttonPressCounter = 0;
unsigned long previous_time = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(1000);
  Serial.println(ssid);
  Serial.println(pass);

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

  pinMode(counterButtonPin, INPUT_PULLUP);
  pinMode(errorButtonPin, INPUT_PULLUP);
  pinMode(errorFreeButtonPin, INPUT_PULLUP);
}

void sendRequest() {
  if(WiFi.status() == WL_CONNECTED) {
    if (client.connect("192.168.1.178", 5001)) {
      client.println("GET /values HTTP/1.1");
      client.println("Host: 10.0.0.59");
      client.println("Connection: close");
      client.println();

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

void loop() {
  unsigned long currentMillis = millis() - previous_time;
  // Button for counter
  if (digitalRead(counterButtonPin) == HIGH) {
    // Increment counter and print
    delay(10);
    if (digitalRead(counterButtonPin) == LOW){
      Serial.println("hello");
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
    Serial.println("");
    Serial.print(buttonPressCounter);
    Serial.print(",");
    Serial.println("WorkOrder");
    sendRequest();
    buttonPressCounter = 0;
  }

  

  delay(5000);
}
