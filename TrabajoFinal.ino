#include <WiFi.h>
#include <WebServer.h>
#include "/src/secrets.h"  // Aquí defines WIFI_SSID y WIFI_PASSWORD

#define LED1 41

WebServer server(80);

bool led1State = false;  // Estado actual del LED

// --- Función para generar el HTML dinámico ---
String getHTML() {
  String led1Text = led1State ? "ON" : "OFF";
  String led1Class = led1State ? "" : "OFF";

  String html = R"(
    <!DOCTYPE html><html>
      <head>
        <title>ESP32 Web Server Demo</title>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <style>
          html { font-family: sans-serif; text-align: center; }
          body { display: inline-flex; flex-direction: column; }
          h1 { margin-bottom: 1.2em; } 
          h2 { margin: 0; }
          div { display: grid; grid-template-columns: 1fr 1fr; grid-template-rows: auto auto; grid-auto-flow: column; grid-gap: 1em; }
          .btn { background-color: #5B5; border: none; color: #fff; padding: 0.5em 1em;
                 font-size: 2em; text-decoration: none }
          .btn.OFF { background-color: #333; }
        </style>
      </head>
      <body>
        <h1>ESP32 Web Server</h1>
        <div>
          <h2>LED 1</h2>
          <a href="/toggle/1" class="btn )" + led1Class + R"(">)" + led1Text + R"(</a>
        </div>
      </body>
    </html>
  )";
  return html;
}

// --- Ruta principal ---
void handleRoot() {
  server.send(200, "text/html", getHTML());
}

// --- Ruta para alternar el LED ---
void handleToggle() {
  if (server.arg(0) == "1" || server.uri().endsWith("/1")) {
    led1State = !led1State;  // Cambia el estado
    digitalWrite(LED1, led1State ? HIGH : LOW);
    Serial.print("LED 1: ");
    Serial.println(led1State ? "ON" : "OFF");
  }
  server.send(200, "text/html", getHTML());
}

void setup() {
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  digitalWrite(LED1, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Conectando a ");
  Serial.println(WIFI_SSID);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Conectado a: ");
  Serial.println(WiFi.SSID());
  Serial.print("Dirección IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/toggle/1", handleToggle);

  server.begin();
  Serial.println("Servidor web iniciado ✅");
}

void loop() {
  server.handleClient();
}
