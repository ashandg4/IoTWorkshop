#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>

// Replace with your Wi-Fi credentials
const char* ssid = "NSUT_WIFI";
const char* password = "";

// DHT setup
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// WebServer on port 80
WebServer server(80);

// HTML + JavaScript (AJAX auto-update)
void handleRoot() {
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <title>ESP32 DHT11 Monitor</title>
      <meta name="viewport" content="width=device-width, initial-scale=1">
      <style>
        body { font-family: Arial; text-align: center; padding: 30px; }
        h2 { font-size: 22px; }
        #data { font-size: 20px; margin-top: 20px; }
      </style>
    </head>
    <body>
      <h2>Temperature & Humidity Monitor</h2>
      <div id="data">Loading...</div>
      <script>
        setInterval(() => {
          fetch("/data")
            .then(response => response.text())
            .then(data => {
              document.getElementById("data").innerHTML = data;
            });
        }, 2000);  // refresh every 2 seconds
      </script>
    </body>
    </html>
  )rawliteral";
  
  server.send(200, "text/html", html);
}

void handleData() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  if (isnan(temperature) || isnan(humidity)) {
    server.send(500, "text/plain", "Sensor Error");
    return;
  }

  String response = "🌡️ Temperature: " + String(temperature) + " °C<br>💧 Humidity: " + String(humidity) + " %";
  server.send(200, "text/html", response);
}

void setup() {
  Serial.begin(115200);
  dht.begin();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi Connected");
  Serial.print("🌐 IP Address: ");
  Serial.println(WiFi.localIP());

  // Define routes
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
  Serial.println("🚀 Web Server Started");
}

void loop() {
  server.handleClient();  // Handle HTTP requests
}
