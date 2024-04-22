String work_order = "abcd";

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
}

void loop() {
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
    Serial.print(work_order);
    Serial.println();
  }
}