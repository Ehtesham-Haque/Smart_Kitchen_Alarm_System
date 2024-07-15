#include <U8glib.h>   // Library for U8glib OLED display.
#include <dht.h>      // Library for DHT11 Sensor.
#include <ArduinoJson.h>


const int Flame_Sensor_Pin = 3;    // Connects to Arduino pin ~3.
const int Smoke_Sensor_Pin = 6;    // Connects to Arduino pin ~6.
const int Gas_Sensor_Pin = 11;     // Connects to Arduino pin ~11.
const int Buzzer_Pin = 10;         // Connects to Arduino pin ~10.
const int Dht_Pin = 5;             // Connects to Arduino pin ~5.
const int Exhaust_Fan_Pin = 9;     // Connects to Arduino pin ~9.

// Threshold values for gas detection & humidity detection.
const int Humidity_Threshold = 60;

// Initialization for OLED display.
U8GLIB_SH1106_128X64 u8g(U8G_I2C_OPT_NONE); // OLED display object
dht DHT;

void setup() {
  Serial.begin(9600);                // Setting Baud rate
  pinMode(Flame_Sensor_Pin, INPUT);  // Takes input from Arduino pin A0.
  pinMode(Smoke_Sensor_Pin, INPUT);  // Takes input from Arduino pin ~6.
  pinMode(Gas_Sensor_Pin, INPUT);    // Takes input from Arduino pin A0.
  pinMode(Buzzer_Pin, OUTPUT);       // Takes output from Arduino pin ~10.
  pinMode(Exhaust_Fan_Pin, OUTPUT);  // Takes output from Arduino pin ~9.
}

void loop() {
  // Reads sensor values.
  int Flame_Value = digitalRead(Flame_Sensor_Pin);  // Reads analog value from Flame sensor.
  int Smoke_Value = digitalRead(Smoke_Sensor_Pin);  // Reads digital value from Smoke sensor.
  int Gas_Value = digitalRead(Gas_Sensor_Pin);      // Reads analog value from Gas sensor.
  DHT.read11(Dht_Pin);   
  
 // Create JSON object
  StaticJsonDocument<200> doc;
  doc["Flame"] = (Flame_Value == LOW) ? "Detected" : "Not Detected";
  doc["Smoke"] = (Smoke_Value == LOW) ? "Detected" : "Not Detected";
  doc["Gas"] = (Gas_Value == LOW) ? "Detected" : "Not Detected";
  doc["Temperature"] = DHT.temperature;
  doc["Humidity"] = DHT.humidity;

  // Serialize JSON to a string
  String jsonString;
  serializeJson(doc, jsonString);

  // Send JSON string over serial
  Serial.println(jsonString);

  delay(1000); // Adjust delay as needed.


  // Update the display with the new sensor readings.
  displayData(Flame_Value, Gas_Value, Smoke_Value);
  // Handle alarms based on sensor readings.
  handleAlarms(Flame_Value, Smoke_Value, Gas_Value);
}


void displayData(int Flame_Value, int Gas_Value, int Smoke_Value) {
  u8g.firstPage();
  do {
    // Display flame status.
    u8g.setFont(u8g_font_unifont);
    u8g.setFont(u8g_font_6x10);
    u8g.drawStr(7, 15, "Flame: ");
    u8g.drawStr(50, 15, Flame_Value == LOW ? "Detected" : "Not Detected");

    // Display gas status.
    u8g.drawStr(7, 45, "Gas: ");
    u8g.drawStr(50, 45, Gas_Value == LOW ? "Detected" : "Not Detected");

    // Display smoke status.
    u8g.drawStr(7, 30, "Smoke: ");
    u8g.drawStr(50, 30, Smoke_Value == LOW ? "Detected" : "Not Detected");

    // Display temperature (T).
    u8g.drawStr(10, 60, "T: ");
    u8g.drawStr(25, 60, String(DHT.temperature, 1).c_str());
    u8g.drawStr(50, 60, "C");

    // Display humidity (H).
    u8g.drawStr(70, 60, "H: ");
    u8g.drawStr(85, 60, String(DHT.humidity, 1).c_str());
    u8g.drawStr(110, 60, "%");
  } while (u8g.nextPage());
}

void handleAlarms(int Flame_Value, int Smoke_Value, int Gas_Value) {
  // Handle gas alarm first since it has priority.
  if (Gas_Value == LOW) {
    tone(Buzzer_Pin, 2280);
    digitalWrite(Exhaust_Fan_Pin, HIGH); // Turn on exhaust fan if gas is detected.
  } 
  // Handle other alarms.
  else if (Flame_Value == LOW) {
    tone(Buzzer_Pin, 1520);
  } 
  else if (Smoke_Value == LOW) {
    tone(Buzzer_Pin, 760);
  } 
  else {
    noTone(Buzzer_Pin); // Turn off buzzer if no alarms are triggered.
    digitalWrite(Exhaust_Fan_Pin, LOW); // Turn off exhaust fan.
  }

  // Turn on exhaust fan if gas is detected.
  digitalWrite(Exhaust_Fan_Pin, DHT.humidity > Humidity_Threshold ? HIGH : LOW);
}