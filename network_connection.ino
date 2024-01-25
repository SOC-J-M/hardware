#include <WiFiS3.h>

#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>
#include <MySQL_Encrypt_Sha1.h>
#include <MySQL_Packet.h>

// specify ssid and password for the connection
char ssid[] = "CalPolyGuest";
char pass[] = "arduinoboard";
char dbHost[] = "106.0.63.154";
char dbUser[] = "pdc1647_Test1101";
char dbPassword[] = "jmpTest1101";
char dbName[] = "pdc1647_Test1101";
// default port number
int port = 3306;

WiFiClient client;
MySQL_Connection conn((Client*)&client);

void setup() {
  // use WiFi.begin(ssid, pass) for other networks
  Serial.begin(115200);
  WiFi.begin(ssid);
 
  while (WiFi.status() != WL_CONNECTED) {
	delay(100);
	Serial.println("test");
	Serial.println(WiFi.status());
  }

  Serial.println("Connected to WiFi");
  // Serial.println(WiFi.localIP());

  if (conn.connect(dbHost, port, dbUser, dbPassword)) {
	Serial.println("Connected to MySQL server");
  } else {
	Serial.println("Connection failed");
  }


}

void loop() {
 
}

