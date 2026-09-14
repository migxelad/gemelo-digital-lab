#include <Arduino.h>
#include <WiFi.h>
#include <ModbusIP_ESP8266.h>

// 1. Credenciales de tu red WiFi doméstica (cámbialas por las tuyas)
const char* ssid = "MIWIFI_2G_qFJW";
const char* password = "3SSu9kTu";

// 2. Pines físicos de nuestra protoboard
const int PIN_BOTON = 4;
const int PIN_RELE  = 23;
const int PIN_LED = 5; 
const int DIRECCION_COIL_LED = 1; // Nuestro nuevo buzón de memoria Modbus

// 3. Objeto Modbus y direcciones de memoria (las que leerá Factory I/O)
ModbusIP mb;
const int DIRECCION_COIL_RELE = 0;   // Dirección 0 para encender el relé (Salida / Coil)
const int DIRECCION_INPUT_BOTON = 0; // Dirección 0 para leer el botón (Entrada discreta)

void setup() {
  // Iniciamos la consola para ver qué pasa por dentro
  Serial.begin(115200);
  
  // Configuración de hardware (igual que en nuestra prueba anterior)
  pinMode(PIN_BOTON, INPUT_PULLUP);
  pinMode(PIN_RELE, OUTPUT);
  pinMode(PIN_LED, OUTPUT);
  mb.addCoil(DIRECCION_COIL_LED, false);
  digitalWrite(PIN_RELE, HIGH); // Apagado por defecto (activo en LOW)

  // Conexión WiFi
  Serial.println("\nConectando al WiFi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // ¡Momento clave! Aquí te dará la IP que necesitas poner en Factory I/O
  Serial.println("\n¡Conectado! La IP de tu ESP32 es: ");
  Serial.println(WiFi.localIP());

  // Configuración Modbus TCP (Modo Servidor/Esclavo)
  mb.server();
  
  // Añadimos los registros a la memoria Modbus del ESP32
  mb.addCoil(DIRECCION_COIL_RELE, false);
  mb.addIsts(DIRECCION_INPUT_BOTON, false);
}

void loop() {
  // 1. Mantiene viva la red Modbus TCP
  mb.task();

  // 2. DEL ESCRITORIO AL SIMULADOR (Botón -> Cinta)
  bool botonPulsado = (digitalRead(PIN_BOTON) == LOW);
  mb.Ists(DIRECCION_INPUT_BOTON, botonPulsado);

  // 3. DEL SIMULADOR AL ESCRITORIO (Sensor -> Actuadores físicos)
  // Leemos ÚNICAMENTE la Coil 0 (donde está enlazado el sensor láser)
  bool estadoSensorVirtual = mb.Coil(DIRECCION_COIL_RELE); 
  
  // Como el láser retrorreflectivo manda un 0 al taparse, usamos el "!" (NOT)
  if (!estadoSensorVirtual) {
    digitalWrite(PIN_RELE, LOW);   // Enciende relé (clack!)
    digitalWrite(PIN_LED, HIGH);   // Enciende el LED rojo
  } else {
    digitalWrite(PIN_RELE, HIGH);  // Apaga relé
    digitalWrite(PIN_LED, LOW);    // Apaga el LED rojo
  }

  // Pausa de estabilidad
  delay(10);
}