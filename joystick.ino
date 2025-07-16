#include <WiFi.h>
#include <WebServer.h>

// Replace with your Wi-Fi credentials
const char* ssid = "NSUT_WIFI";
const char* password = "";

// Joystick pins
const int joyX = 34;
const int joySW = 27;

WebServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(joySW, INPUT_PULLUP);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
  Serial.println(WiFi.localIP());

  server.on("/", handleGamePage);
  server.on("/data", handleJoystick);

  server.begin();
}

void loop() {
  server.handleClient();
}

void handleJoystick() {
  int x = analogRead(joyX);
  int sw = digitalRead(joySW);
  String json = "{\"x\":" + String(x) + ",\"sw\":" + String(sw) + "}";
  server.send(200, "application/json", json);
}

void handleGamePage() {
  server.send(200, "text/html", R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>Dodge the Boxes</title>
  <style>
    body { margin: 0; overflow: hidden; background: #000; }
    canvas { display: block; margin: auto; background: #111; }
  </style>
</head>
<body>
  <canvas id="game" width="320" height="480"></canvas>
  <script>
    const canvas = document.getElementById("game");
    const ctx = canvas.getContext("2d");

    let player = { x: 160, y: 440, w: 40, h: 10 };
    let blocks = [];
    let score = 0;

    function addBlock() {
      blocks.push({ x: Math.random() * 280, y: 0, w: 40, h: 10 });
    }

    function draw() {
      ctx.clearRect(0, 0, canvas.width, canvas.height);
      ctx.fillStyle = "lime";
      ctx.fillRect(player.x, player.y, player.w, player.h);

      ctx.fillStyle = "red";
      for (let b of blocks) {
        ctx.fillRect(b.x, b.y, b.w, b.h);
      }

      ctx.fillStyle = "white";
      ctx.fillText("Score: " + score, 10, 20);
    }

    function update() {
      for (let b of blocks) {
        b.y += 4;
        // collision check
        if (
          b.x < player.x + player.w &&
          b.x + b.w > player.x &&
          b.y < player.y + player.h &&
          b.y + b.h > player.y
        ) {
          alert("💥 Game Over!\nScore: " + score);
          location.reload();
        }
      }
      blocks = blocks.filter(b => b.y < 480);
    }

    function loop() {
      fetch('/data')
        .then(res => res.json())
        .then(data => {
          const xVal = data.x;
          // Map 0–4095 → 0–280
          let px = map(xVal, 0, 4095, 0, 280);
          player.x = px;
        });
    }

    function map(x, in_min, in_max, out_min, out_max) {
      return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

    setInterval(() => {
      addBlock();
      score++;
    }, 1200);

    setInterval(() => {
      update();
      draw();
    }, 60);

    setInterval(loop, 100);
  </script>
</body>
</html>
  )rawliteral");
}
