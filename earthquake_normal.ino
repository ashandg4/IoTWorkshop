#include <WiFi.h>
#include <WebServer.h>

// Wi-Fi credentials
const char* ssid = "NSUT_WIFI";
const char* password = "";

// Sensor pins
#define VIBRATION_PIN 34
#define TILT_PIN 35

// Web server setup
WebServer server(80);

// Timing
unsigned long vibrationTime = 0;
unsigned long tiltTime = 0;
unsigned long lastEvent = 0;

const int HOLD_DURATION = 1000; // ms to keep event "active"

// State tracking
bool vibrationDetected = false;
bool tiltDetected = false;

String status = "Idle";
String severity = "None";

// HTML generator
String generateHTML() {
  String html = "<!DOCTYPE html><html><head><meta http-equiv='refresh' content='1'>";
  html += "<title>Earthquake Detector</title></head><body style='font-family:sans-serif;text-align:center;'>";
  html += "<h1>ESP32 Earthquake Detector</h1>";
  html += "<p style='font-size:1.5em;'>Status: <strong>" + status + "</strong></p>";
  html += "<p style='font-size:1.2em;'>Severity: " + severity + "</p>";
  html += "<p>KY-002 Vibration Sensor & KY-017 Tilt Sensor</p>";
  html += "</body></html>";
  return html;
}

// Root handler
void handleRoot() {
  server.send(200, "text/html", generateHTML());
}

void setup() {
  Serial.begin(115200);

  pinMode(VIBRATION_PIN, INPUT);
  pinMode(TILT_PIN, INPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! IP: " + WiFi.localIP().toString());

  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  unsigned long now = millis();

  // Check vibration pulse
  if (digitalRead(VIBRATION_PIN) == HIGH) {
    vibrationDetected = true;
    vibrationTime = now;
    Serial.println("Vibration Detected");
  }

  // Check tilt pulse
  if (digitalRead(TILT_PIN) == HIGH) {
    tiltDetected = true;
    tiltTime = now;
    Serial.println("Tilt Detected");
  }

  // Expire old events after hold time
  if (now - vibrationTime > HOLD_DURATION) vibrationDetected = false;
  if (now - tiltTime > HOLD_DURATION) tiltDetected = false;

  // Set status and severity
  if (vibrationDetected && tiltDetected) {
    status = "Earthquake Detected!";
    severity = "High";
  } else if (vibrationDetected) {
    status = "Shock Detected";
    severity = "Moderate";
  } else if (tiltDetected) {
    status = "Tilt Detected";
    severity = "Low";
  } else {
    status = "Idle";
    severity = "None";
  }

  server.handleClient();
}