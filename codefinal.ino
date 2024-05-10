#include <WiFiS3.h>
#include <LiquidCrystal_I2C.h>
#include <ctype.h>

// prev time
unsigned long prevTime = 0;
// cycle time
unsigned long currTime = 0;
// push to record time (10 sec)
unsigned long pushTime = 20000;
// previous count
unsigned long prevCount = 0;
// current count
unsigned long currCount = 0;
// work order
String workOrder = "Awaiting Order...";
// part number
String partNumber = "0";
// status
String statusCode = "0";
// reporting node
String reportingNode = "Arduino_1";
// department
String department = "dept";
// work center location
String centerLocation = "ASSY_1";
// employee id (initialized to null)
String employeeId = "null";
// sequence (initialized to null)
String sequence = "null";
// order completion status
String orderStatus = "PARTIAL";
// setup duration time (default to 0)
unsigned long setupTime = 0;
// DIE SET #
String dieSet = "null";
// material lot #
String materialLot = "null";

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
  
  Serial.println(workOrder);
  // display awaiting work order
  while (workOrder == "Awaiting Order..."){
    setupScan(); 
  }

  setupScan();
  Serial.println(workOrder);
  updateLCD();
  setupTime = millis();
}

// scan function only used in the initial setup of the arduino
// polls Serial1 until a code is scanned
// verifies that the code is a workOrder and then sets the workOrder variable
void setupScan(){
  if (Serial1.available()){
    String scannedOrder = Serial1.readString();
    if (isDigit(scannedOrder.charAt(0))){
      workOrder = scannedOrder.substring(0, scannedOrder.length() - 1);
    }
  }
}

void pushRecord() {
  // connect to the api
  // calculate how many parts were created in this cycle (current - previous)
  // client.println("POST ...")
  // set previous = current
  Serial.println("Entered PushRecord()");
  String partsMade = String(currCount - prevCount);
  //prevTime = currTime;
  //USE CODE FROM THE PREVIOUS IMPLEMENTATION
  // display "Record Pushed"
  if(WiFi.status() == WL_CONNECTED) {
    if (client.connect("192.168.1.178", 5001)) { 
      // MODIFY THE QUERY BELOW TO PUSH ALL FIELDS IN DB
      String cycleTime = String(currTime - prevTime);
      Serial.println(cycleTime);
      String testRecord = "POST /dataQuery?department=" + department + "&work_center=" + centerLocation + "&employee_id=" + employeeId + "&part_number=" + partNumber + "&quantity=" + partsMade + "&work_order="+ workOrder + "&sequence_num=" + sequence + "&progress=" + orderStatus + "&setup_time=" + String(setupTime) + "&die_set=" + dieSet + "&material_lot=" + materialLot + "&status_code=" + statusCode + "&comments=random&reporting_node=" + reportingNode + "&elapsed_time=" + cycleTime + " HTTP/1.1";
      //String record = “POST /dataQuery?department=” + department + “&work_center=”+ centerLocation + “&employee_id=” + employeeId + “&part_number=” + partNumber + “&quantity=” + partsMade + “&work_order=helowork” + “&sequence_num=” + sequence + “&progress=” + orderStatus + “&setup_time=” + String(setupTime) + “&die_set=” dieSet + “&material_lot=” + materialLot + “&status_code=” + statusCode + “&comments=none&reporting_node=” + reportingNode + “&elapsedTime=” + cycleTime + “ HTTP/1.1”;
      client.println(testRecord);
      //client.println("Host: 10.0.0.59");
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
      prevTime = currTime;
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
  //Serial.println("validateScan");
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


String formatTime() {
  // calculate hours, minutes, and seconds
  unsigned long totalSeconds = (currTime - setupTime) / 1000;
  unsigned long hours = totalSeconds / 3600;
  unsigned long minutes = (totalSeconds % 3600) / 60;
  unsigned long seconds = totalSeconds % 60;
  
  // format the time as "HH:MM:SS"
  String formattedTime = "";
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
  //Serial.println("updateLCD");
  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print("WO: ");
  lcd.print(workOrder);

  lcd.setCursor(0, 1);
  lcd.print("Elapsed: ");
  lcd.print(formatTime());
  
  lcd.setCursor(0, 2);
  lcd.print("Status: ");
  lcd.print(statusCode);
}


void loop() {
  updateLCD();
  validateScan();
  currTime = millis();

  // check if record needs to be pushed
  if ((currTime - prevTime) > pushTime) {
    pushRecord();
  }

  // check for counter increment
  if (digitalRead(counterPin) == HIGH) { // button has been pressed
    while (digitalRead(counterPin) == HIGH); // debounce; will stay in this loop until the counter is de-pressed
    // counter is back to LOW
    currCount++;
    Serial.println(currCount);
  }

  

  
  
}
