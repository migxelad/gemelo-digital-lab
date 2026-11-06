#include <Arduino.h>
#include <WiFi.h>
#include <ModbusIP_ESP8266.h>

#if __has_include("secrets.h")
#include "secrets.h"
#else
#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASSWORD "YOUR_WIFI_PASSWORD"
#endif

// Physical bench I/O
constexpr uint8_t PIN_BOTON = 4;
constexpr uint8_t PIN_RELE = 23;
constexpr uint8_t PIN_LED = 5;

constexpr uint16_t DIRECCION_COIL_LED = 1;
constexpr uint16_t DIRECCION_COIL_RELE = 0;
constexpr uint16_t DIRECCION_INPUT_BOTON = 0;

ModbusIP mb;

void setup() {
  Serial.begin(115200);

  pinMode(PIN_BOTON, INPUT_PULLUP);
  pinMode(PIN_RELE, OUTPUT);
  pinMode(PIN_LED, OUTPUT);

  mb.addCoil(DIRECCION_COIL_LED, false);
  mb.addCoil(DIRECCION_COIL_RELE, false);
  mb.addIsts(DIRECCION_INPUT_BOTON, false);
  mb.server();

  digitalWrite(PIN_RELE, HIGH); // Relay is active-low; default off.
  digitalWrite(PIN_LED, LOW);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("Connecting to Wi-Fi...");
}

void loop() {
  mb.task();

  static bool reportedConnection = false;
  if (WiFi.status() == WL_CONNECTED) {
    if (!reportedConnection) {
      Serial.print("ESP32 IP address: ");
      Serial.println(WiFi.localIP());
      reportedConnection = true;
    }
  } else {
    reportedConnection = false;
  }

  const bool buttonPressed = (digitalRead(PIN_BOTON) == LOW);
  mb.Ists(DIRECCION_INPUT_BOTON, buttonPressed);

  const bool virtualSensorState = mb.Coil(DIRECCION_COIL_RELE);
  digitalWrite(PIN_RELE, virtualSensorState ? HIGH : LOW);
  digitalWrite(PIN_LED, virtualSensorState ? LOW : HIGH);
}
