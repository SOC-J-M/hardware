int x;

void setup() {
  Serial.begin(115200);
  
  Serial.setTimeout(1);
}

void loop() {
  Serial.println("Hello\n");
}
