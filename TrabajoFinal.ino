#include <WiFi.h>
#include <WebServer.h>
#include "src/secrets.h"
#include "DHT.h"

#define LED1 41
#define DHTPIN 2
#define DHTTYPE DHT22

WebServer server(80);
DHT dht(DHTPIN, DHTTYPE);
bool led1State = false;

// --- Función para CORS y enviar JSON ---
void sendJSONResponse(const String &json) {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
  server.sendHeader("Access-Control-Allow-Headers", "*");
  server.send(200, "application/json", json);
}

// --- Rutas ---
void handleRoot() {
  server.send(200, "text/plain", "Servidor ESP32 IoT activo ✅");
}

void handleToggle() {
  if (server.arg(0) == "1" || server.uri().endsWith("/1")) {
    led1State = !led1State;
    digitalWrite(LED1, led1State ? HIGH : LOW);
    Serial.printf("LED 1: %s\n", led1State ? "ON" : "OFF");
    String json = "{\"status\":\"ok\",\"ledState\":" + String(led1State ? "true" : "false") + "}";
    sendJSONResponse(json);
  } else {
    sendJSONResponse("{\"error\":\"LED no válido\"}");
  }
}

void handleSensorData() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (isnan(temp) || isnan(hum)) {
    sendJSONResponse("{\"error\":\"Fallo al leer el sensor\"}");
    return;
  }

  String json = "{\"temperature\":" + String(temp, 1) + ",\"humidity\":" + String(hum, 0) + "}";
  sendJSONResponse(json);
}

// --- Setup ---
void setup() {
  Serial.begin(115200);
  pinMode(LED1, OUTPUT);
  digitalWrite(LED1, LOW);
  dht.begin();

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.printf("Conectando a %s", WIFI_SSID);
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }

  Serial.println("\n✅ WiFi conectado");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/toggle/1", handleToggle);
  server.on("/api/sensor", handleSensorData);

  server.begin();
  Serial.println("Servidor web iniciado ✅");
}

// --- Loop ---
void loop() {
  server.handleClient();
}
