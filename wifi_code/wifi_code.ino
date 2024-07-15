
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL6wCPp3lQi"
#define BLYNK_TEMPLATE_NAME "Smart Kitchen and Alarm System"
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include <BlynkSimpleEsp8266.h>

char auth[] = "";  // Blynk authentication token
char ssid[] = ""; // WiFi network SSID
char pass[] = ""; // WiFi network password
// Virtual pin numbers
#define FLAME_PIN V0
#define SMOKE_PIN V1
#define GAS_PIN V2
#define TEMP_PIN V3
#define HUMIDITY_PIN V4
void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass); // Connect to Blynk server
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
void loop() {
  Blynk.run();
  if (Serial.available() > 0) {
    String data = Serial.readStringUntil('\n');
    parseSensorData(data); // Parse and process sensor data
  }
}
void parseSensorData(String data) {
  Serial.println("Received data: " + data); // Print received data
  // Parse JSON data
  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, data);
  if (error) {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }
  // Update Blynk widgets with sensor data
  Blynk.virtualWrite(FLAME_PIN, doc["Flame"].as<String>());
  Blynk.virtualWrite(SMOKE_PIN, doc["Smoke"].as<String>());
  Blynk.virtualWrite(GAS_PIN, doc["Gas"].as<String>());
  Blynk.virtualWrite(TEMP_PIN, doc["Temperature"].as<float>());
  Blynk.virtualWrite(HUMIDITY_PIN, doc["Humidity"].as<float>());
}
