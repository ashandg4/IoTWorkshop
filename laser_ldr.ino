#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "NSUT_WIFI";
const char* password = "";

#define LDR_PIN 34 // Analog input pin for LDR

WebServer server(80);

// Threshold may vary — test and adjust
int threshold = 1500;

String getStatus() {
  int ldrValue = analogRead(LDR_PIN);
  Serial.println("LDR Value: " + String(ldrValue));

  if (ldrValue > threshold) {
    return "<h2 style='color:red;'>⚠ Laser Beam Interrupted</h2>";
  } else {
    return "<h2 style='color:green;'>✅ Beam Intact</h2>";
  }
}

String buildPage() {
  String html = R"rawliteral(
    <!DOCTYPE html><html>
    <head>
      <meta http-equiv='refresh' content='0.2'>
      <title>Laser Tripwire Alert</title>
    </head>
    <body style='text-align:center; font-family:sans-serif;'>
      <h1>ESP32 Laser Tripwire</h1>
  )rawliteral";
  html += getStatus();
  html += R"rawliteral(
      <p>Status updates every 200ms</p>
    </body></html>
  )rawliteral";
  return html;
}

void setup() {
  Serial.begin(115200);
  pinMode(LDR_PIN, INPUT);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }

  Serial.println("\nConnected. IP: " + WiFi.localIP().toString());

  server.on("/", []() {
    server.send(200, "text/html", buildPage());
  });

  server.begin();
  Serial.println("Webserver started");
}

void loop() {
  server.handleClient();
}