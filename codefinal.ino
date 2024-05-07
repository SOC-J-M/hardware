#include <WiFiS3.h>
#include <LiquidCrystal_I2C.h>
#include <ctype.h>

// global time
unsigned long timeGlobal = 0
// cycle time
unsigned long cycleTime = millis()
unsigned long pushTime = 10000
// previous count
unsigned long prevCount = 0
// current count
unsigned long currCount = 0
// work order
String workOrder = "Awaiting Order..."
// status
int statusCode = 0
// reporting node
String reportingNode = "Arduino 1"
// work center location
String centerLocation = "ASSY_1"
// employee id (initialized to null)
String employeeId = "null"
// sequence (initialized to null)
String sequence = "null"
// order completion status
String orderStatus = "PARTIAL"
// setup duration time (default to 0)
unsigned long setupDuration = 0
// DIE SET #
String dieSet = "null"
// material lot #
String materialLot = "null"

// initialize wifi attributes
WiFiClient client;
int status = WL_IDLE_STATUS;

// initialize button pin
int counterPin = 5;

// intialize LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);

const char ssid[] = "SpectrumSetup-7D"; // Wifi network
const char pass[] = "manygate969";  // Password, if open network you can leave blank.

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  pinMode(counterPin, INPUT);

  lcd.init();         // Initialize the LCD
  lcd.backlight();    // Turn on the backlight

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

  // display awaiting work order
  while (work_order == "Awaiting Order..."){
    Serial.print(work_order);
    setupScan();
    delay(1000);
  }
  
}

// scan function only used in the initial setup of the arduino
// polls Serial1 until a code is scanned
// verifies that the code is a workOrder and then sets the workOrder variable
void setupScan(){
  if (Serial1.available()){
    String scannedOrder = Serial1.readString();
    if (isDigit(scannedMessage.charAt(0))){
      workOrder = scannedOrder;
    }
  }
}

void pushRecord() {
  // connect to the api
  // calculate how many parts were created in this cycle (current - previous)
  // client.println("POST ...")
  // set previous = current
  int partsMade = currCount - prevCount;
  
  //USE CODE FROM THE PREVIOUS IMPLEMENTATION
  // display "Record Pushed"
  if(WiFi.status() == WL_CONNECTED) {
    if (client.connect("192.168.1.178", 5001)) {
      // MODIFY THE QUERY BELOW TO PUSH ALL FIELDS IN DB
      // client.println("POST /dataQuery?quantity=" + String(buttonPressCounter) + "&work_order=" + String(work_order) + " HTTP/1.1");
      client.println("Host: 10.0.0.59");
      client.println("Connection: close");
      client.println();
      // for debugging
      while (client.connected()) {
        if (client.available()) {
          Serial.write(client.read());
        }
      }
      client.stop();
      // reset cycle time to 0
      timeGlobal = cycleTime;
      // reset quantities
      prevCount = currCount;
    } else {
      Serial.println("Failed to connect to server.");
    }
  } else {
    Serial.println("WiFi not connected.");
  }
}


// this function is ONLY to validate scans during the runtime of the work order
// once something is scanned
// this function assumes that whatever is scanned is either an error code or a status code
void validateScan() {
  // Serial.read() OR WE CAN PASS THIS IN FROM LOOP
  if (Serial1.available()){
    String scannedMessage = Serial1.readString();
    // if scan is a new work order
    // if the first character is a digit, then the scanned message is a work order
    if (isDigit(scannedMessage.charAt(0))){
      // set the record complete instead of partial ** technically not always correct but its a fair assumption
      orderStatus = "COMPLETE";
      // push the last record
      pushRecord();
      // set work order to the new work order
      workOrder = scannedMessage;
    } else {
      // else the scan is an error code (check if the first two characters are 'X'
      // push the last record
      pushRecord();
      // set status to the new error
      statusCode = scannedMessage;
      // continue with runtime
    }
  }
}


void formatTime() {
  // calculate hours, minutes, and seconds
  unsigned long totalSeconds = timeGlobal / 1000;
  unsigned long hours = totalSeconds / 3600;
  unsigned long minutes = (totalSeconds % 3600) / 60;
  unsigned long seconds = totalSeconds % 60;
  
  // format the time as "HH:MM:SS"
  formattedTime = "";
  if (hours < 10) {
    formattedTime += "0";
  }
  formattedTime += String(hours) + ":";
  
  if (minutes < 10) {
    formattedTime += "0";
  }
  formattedTime += String(minutes) + ":";
  
  if (seconds < 10) {
    formattedTime += "0";
  }
  formattedTime += String(seconds);
  return formattedTime;
}


void updateLCD() {
  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print("Work Order: ");
  lcd.print(workOrder);

  lcd.setCursor(0, 1);
  lcd.print("Time on job: ");
  lcd.print(formatTime());
  
  lcd.setCursor(0, 2);
  lcd.print("Status: ");
  lcd.print(statusCode);
}


void loop() {
  validateScan();

  // check if record needs to be pushed
  if ((cycleTime - timeGlobal) > pushTime) {
    pushRecord();
  }

  // check for counter increment
  if (digitalRead(counterPin) == HIGH) { // button has been pressed
    while (digitalRead(counterPin) == HIGH); // debounce; will stay in this loop until the counter is de-pressed
    // counter is back to LOW
    currCount++;
    break;
  }

  updateLCD();
  
}
