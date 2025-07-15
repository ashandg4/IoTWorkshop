#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "NSUT_WIFI";
const char* password = "";

#define PIR_PIN 15   // PIR sensor output to GPIO15
//#define BUZZER_PIN 14  // Optional buzzer pin

WebServer server(80);

String getMotionStatus() {
  int motion = digitalRead(PIR_PIN);
  if (motion == HIGH) {
    //digitalWrite(BUZZER_PIN, HIGH); // Optional buzzer
    return "<h2 style='color:red;'>⚠️ Motion Detected!</h2>";
  } else {
    //digitalWrite(BUZZER_PIN, LOW);
    return "<h2 style='color:green;'>✅ No Motion</h2>";
  }
}

String htmlPage() {
  String html = R"rawliteral(
    <!DOCTYPE html><html>
    <head>
      <meta http-equiv="refresh" content="0.2"> <!-- Auto refresh every 1s -->
      <title>PIR Motion Tracker</title>
    </head>
    <body style="text-align:center; font-family:sans-serif;">
      <h1>PIR Motion Tracker (ESP32)</h1>
  )rawliteral";

  html += getMotionStatus();
  html += R"rawliteral(
      <p>Status updates every second</p>
    </body></html>
  )rawliteral";

  return html;
}

void handleRoot() {
  server.send(200, "text/html", htmlPage());
}

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);
  //pinMode(BUZZER_PIN, OUTPUT);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }

  Serial.println("\nConnected to WiFi. IP: " + WiFi.localIP().toString());

  server.on("/", handleRoot);
  server.begin();
  Serial.println("Webserver started.");
}

void loop() {
  server.handleClient();
}
