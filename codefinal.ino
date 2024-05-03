#include <WiFiS3.h>
#include <LiquidCrystal_I2C.h>

// global time
// cycle time
// previous count
// current count
// work order
// status



const char ssid[] = "SpectrumSetup-7D"; // Wifi network
const char pass[] = "manygate969";  // Password, if open network you can leave blank.

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
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
  
}


void pushRecord() {
  // connect to the api
  // calculate how many parts were created in this cycle (current - previous)
  // client.println("POST ...")
  // set previous = current
  // set cycleTime = 0

  //USE CODE FROM THE PREVIOUS IMPLEMENTATION
  // display "Record Pushed"
}


// this function is ONLY to validate scans during the runtime of the work order
// once something is scanned
// this function assumes that whatever is scanned is either an error code or a status code
void validateLoopScan() {
  // Serial.read() OR WE CAN PASS THIS IN FROM LOOP
  // if the scan is an error code (check if the first two characters are 'X'
    // push the last record
    // set status to the new error
    // continue with runtime
  // else scan is a new work order
    // set the record complete instead of partial ** technically not always correct but its a fair assumption
    // push the last record
    // set work order to the new work order
    // reset both times to 0
    // reset both quantity variables to 0
}



void loop() {
  // if Serial1.available()
    // then validateLoopScan
  // count button clicks
}
