#include <WiFi.h>
#include <WebServer.h>

// Wi-Fi credentials
const char* ssid = "NSUT_WIFI";
const char* password = "";

// Sensor pins
#define TILT_PIN 35
#define TRACKING_PIN 34

// Web server setup
WebServer server(80);

// Timing
unsigned long tiltTime = 0;
unsigned long trackingTime = 0;

const int HOLD_DURATION = 1000; // ms to keep event "active"

// State tracking
bool tiltDetected = false;
bool trackingDetected = false;

String status = "Idle";
String severity = "None";

// HTML generator
String generateHTML() {
  String html = "<!DOCTYPE html><html><head><meta http-equiv='refresh' content='1'>";
  html += "<title>Tilt & Tracking Detector</title></head><body style='font-family:sans-serif;text-align:center;'>";
  html += "<h1>ESP32 Tilt + Tracking Sensor</h1>";
  html += "<p style='font-size:1.5em;'>Status: <strong>" + status + "</strong></p>";
  html += "<p style='font-size:1.2em;'>Severity: " + severity + "</p>";
  html += "<p>Using KY-017 Tilt Sensor & IR Tracking Module</p>";
  html += "</body></html>";
  return html;
}

// Root handler
void handleRoot() {
  server.send(200, "text/html", generateHTML());
}

void setup() {
  Serial.begin(115200);

  pinMode(TILT_PIN, INPUT);
  pinMode(TRACKING_PIN, INPUT);

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

  // Check tilt sensor
  if (digitalRead(TILT_PIN) == HIGH) {
    tiltDetected = true;
    tiltTime = now;
    Serial.println("Tilt Detected");
  }

  // Check tracking sensor
  if (digitalRead(TRACKING_PIN) == LOW) {
    trackingDetected = true;
    trackingTime = now;
    Serial.println("Tracking Line/Object Detected");
  }

  // Expire old events
  if (now - tiltTime > HOLD_DURATION) tiltDetected = false;
  if (now - trackingTime > HOLD_DURATION) trackingDetected = false;

  // Set status and severity
  if (tiltDetected && trackingDetected) {
    status = "Tilt + Tracking Detected!";
    severity = "High";
  } else if (tiltDetected) {
    status = "Tilt Only";
    severity = "Moderate";
  } else if (trackingDetected) {
    status = "Tracking Only";
    severity = "Low";
  } else {
    status = "Idle";
    severity = "None";
  }

  server.handleClient();
}
