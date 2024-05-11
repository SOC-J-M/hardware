#include <WiFiS3.h>
#include <LiquidCrystal_I2C.h>
#include <ctype.h>

unsigned long prevTime = 0;               // pointer to current cycle start time
unsigned long currTime = 0;               // updated to millis() in loop
unsigned long setupTime = 0;              // pointer to start time of work order
String formattedTime = "---";             // format HH:MM:SS
unsigned long pushTime = 600000;          // push a record every 10 min
int debounceDelay = 1000;                  // debounce for recording button presses
unsigned long lastCounterTime = 0;        // pointer to last count made (for debounce)
unsigned long prevCount = 0;              // pointer to counter at current cycle start time
unsigned long currCount = 0;              // current overall count

String department = "dept";               // department
String workCenter = "ASSY_1";             // work center
String employeeId = "null";               // employee id
String partNumber = "0";                  // part number
String partsMade = "0";                   // parts made per cycle
String workOrder = "---";                 // work order number
String sequence = "null";                 // sequence
String orderStatus = "PARTIAL";           // order status
String dieSet = "null";                   // die set number
String materialLot = "null";              // material lot number
String statusCode = "NORMAL";             // status code
String reportingNode = "Arduino_1";       // reporting node
String cycleTime = "0";                   // cycle time

int counterPin = 5;                       // part counter

// **UPDATE THESE VARIABLES**
const char ssid[] = "SpectrumSetup-7D";   // WiFi network
const char pass[] = "manygate969";        // password, if open network you can leave blank
const char serverIP[] = "192.168.1.177";  // ip of api server
int serverPort = 5001;                    // port of api server

// initialize WiFi client
WiFiClient client;
int status = WL_IDLE_STATUS;

// initialize LCD
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  pinMode(counterPin, INPUT);             // set counter pin
  lcd.init();                             // Initialize the LCD
  lcd.backlight();                        // Turn on the backlight

  // connect to WiFi
  WiFi.begin(ssid, pass);                 // if open network remove pass arg
  Serial.print("Connecting to " + String(ssid));
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to " + String(ssid));
  
  updateLCD();
  
  // wait for work order to be scanned
  while (workOrder == "---"){
    if (Serial1.available()) {
      String scanned = Serial1.readString();
      scanned.trim();
      if (isDigit(scanned.charAt(0))){ // check if valid work order
        workOrder = scanned;
      }
    }
  }
  
  // initialize pointers to start time of work order
  setupTime = millis();
  prevTime = millis();
}

void loop() {
  // update current time
  currTime = millis();
  
  // check if time needs to be updated
  if (formatTime() != formattedTime) {
    formattedTime = formatTime();
    updateLCD();
  }

  // check if new scan on serial
  if (Serial1.available()) {
    validateScan();
  }
  
  // check if record needs to be pushed
  if ((currTime - prevTime) > pushTime) {
    pushRecord();
  }

  // check for counter increment
  if (digitalRead(counterPin) == HIGH) { // button has been pressed
    if ((currTime - lastCounterTime) > debounceDelay) { // check for debounce
      currCount++;
      lastCounterTime = currTime;
      Serial.println(currCount);
    }
  }
}

void pushRecord() {
  if (client.connect(serverIP, serverPort)) {   // connect to API
    partsMade = String(currCount - prevCount); // get quantity made in current cycle
    cycleTime = String(currTime - prevTime); // get current time in cycle

    String record = "POST /dataQuery?department=" + department + 
                "&work_center=" + workCenter + 
                "&employee_id=" + employeeId + 
                "&part_number=" + partNumber + 
                "&quantity=" + partsMade + 
                "&work_order="+ workOrder + 
                "&sequence_num=" + sequence + 
                "&progress=" + orderStatus + 
                "&setup_time=" + String(setupTime) + 
                "&die_set=" + dieSet + 
                "&material_lot=" + materialLot + 
                "&status_code=" + statusCode + 
                "&comments=random&reporting_node=" + reportingNode + 
                "&elapsed_time=" + cycleTime + " HTTP/1.1";
  
    // send POST request to API
    client.println(record); 
    client.println("Connection: close");
    client.println();
    
    prevTime = currTime;    // update time pointer for new cycle
    prevCount = currCount;  // update counter pointer (i.e. set quantity to 0 for new cycle)
  } else {
    Serial.println("Failed to connect to server.");
  }
}

// checks if whatever is scanned is valid (work order or error code)
void validateScan() {
  String scanned = Serial1.readString();
  scanned.trim();
 
  if (isDigit(scanned.charAt(0)) && !scanned.equals(workOrder)) { // check if new work order
    orderStatus = "COMPLETE";                 
    pushRecord();                             // push the most recent record
    workOrder = scanned;                      // set new work order variables
    orderStatus = "PARTIAL";                 
    statusCode = "NORMAL";                    
    setupTime = millis();
    prevCount = 0;
    currCount = 0;
  } else if (scanned.startsWith("status")) {  // check if status
    pushRecord();                             // push the last record
    statusCode = scanned.substring(6);        // update status code
  }
  // otherwise, the scan is ignored (neither work order or error code)
}

String formatTime() {
  unsigned long totalSeconds = (currTime - setupTime) / 1000;
  unsigned long hours = totalSeconds / 3600;
  unsigned long minutes = (totalSeconds % 3600) / 60;
  unsigned long seconds = totalSeconds % 60;
  String temp = "";

  if (hours < 10) {
    temp += "0";
  }
  temp += String(hours) + ":";
  
  if (minutes < 10) {
    temp += "0";
  }
  temp += String(minutes) + ":";
  
  if (seconds < 10) {
    temp += "0";
  }
  temp += String(seconds);
  
  return temp;
}


void updateLCD() {
  lcd.clear();
  
  lcd.setCursor(0, 0);
  lcd.print("Order Number: ");
  lcd.print(workOrder);

  lcd.setCursor(0, 1);
  lcd.print("Elapsed: ");
  lcd.print(formattedTime);
  
  lcd.setCursor(0, 2);
  lcd.print("Status: ");
  // split specific long status codes into two lines
  if (statusCode == "RAW MATERIAL BAD") {
    lcd.print("RAW MATERIAL");
    lcd.setCursor(0, 3);
    lcd.print("BAD");
  } else if (statusCode == "RAW MATERIAL NOT AVAILABLE") {
    lcd.print("RAW MATERIAL");
    lcd.setCursor(0, 3);
    lcd.print("NOT AVAILABLE");
  } else if (statusCode == "TOOLING NOT AVAILABLE") {
    lcd.print("TOOLING");
    lcd.setCursor(0, 3);
    lcd.print("NOT AVAILABLE");
  } else {
    lcd.print(statusCode);
  }
}
