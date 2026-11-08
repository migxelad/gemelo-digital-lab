# Línea de embotellado inteligente — Centro del gemelo digital e integración HIL

## Resumen

Gemelo digital de una célula de embotellado flexible que combina tecnología operacional e informática: Machine Simulator Suite v5 (Nirtec), Siemens TIA Portal V21 con S7-PLCSIM en VMware, un nodo de campo ESP32 mediante Modbus TCP y una capa de supervisión Edge prevista. El repositorio reúne el contexto del proyecto, exportaciones de texto del PLC, firmware PlatformIO, la escena de Machine Simulator y documentación de ingeniería.

## Topología de comunicaciones

```text
┌────────────────────── PC anfitrión ─────────────────────────────┐
│ Machine Simulator Suite v5                                      │
│                 │ S7 TCP/IP                                     │
│                 ▼                                               │
│ NetToPLCsim — TCP 102, Rack 0 / Slot 1                          │
│                 │                                               │
│        Red VMware en modo puente                                │
│                 ▼                                               │
│ ┌────────────── VM ──────────────┐      ┌── Nodo ESP32 ────────┐│
│ │ TIA Portal V21 + S7-PLCSIM     │      │ Modbus TCP, puerto 502││
│ └────────────────────────────────┘      └──────────────────────┘│
└─────────────────────────────────────────────────────────────────┘
                                      ··· supervisión Edge futura
```

Configura la red de la VM en modo puente cuando el equipo anfitrión, la pasarela del PLC virtual y los dispositivos deban comunicarse en la misma LAN. NetToPLCsim expone la conexión S7 simulada en TCP 102 con Rack 0 / Slot 1. El ESP32 ofrece Modbus TCP en el puerto 502.

## Contrato de memoria

Todas las E/S digitales de proceso entre TIA Portal y Machine Simulator / HIL comienzan en el byte 10: `%I10.0` y `%Q10.0`. Los bytes anteriores se reservan para evitar colisiones con la memoria de ciclo de la CPU o del sistema. No asignes señales de proceso a `%I0.x` ni `%Q0.x`. Consulta las [reglas para agentes y el repositorio](docs/internal/agentes/AGENTS.md).

## Estaciones del proceso

- **Regulación del nivel del tanque:** PID_Compact en una tarea cíclica forma parte de la arquitectura propuesta; en este repositorio todavía no consta como validado el uso de E/S físicas ni el funcionamiento en lazo cerrado.
- **Entrada y buffer:** el transportador y la retención de un único envase están definidos en el contexto maestro. Antes de implementar E/S adicionales, verifica los actuadores actuales del simulador y las etiquetas del PLC.
- **Clasificación por altura:** validada en movimiento con tres fotocélulas discretas (S/M/L) y un disparador de presencia. El código SCL prioriza L → M → S y actualiza la receta activa sin detener el transportador. Es una medición discreta, no una inspección analógica.
- **Llenado y báscula de calidad:** están previstos el llenado por receta y un tiempo de estabilización de 1,0 s en la báscula. Falta confirmar la escala de la señal de peso, los pesos objetivo, la secuencia PASS/REJECT y la validación del hardware.
- **Célula de packaging:** la cinemática final y la secuencia siguen pendientes de decisión.

## Demostraciones y documentación de ingeniería

- [Índice de vídeos y enlaces externos](docs/videos/README.md). Los vídeos originales se retiraron de Git; aún faltan los enlaces externos y las miniaturas GIF de las estaciones.
- [Diagrama GRAFCET de la estación buffer](docs/diagrams/GRAFCET_BufferStation.jpg)
- [Escena de Machine Simulator](machine_simulator/GemeloDigitalEnsamblado.maq)
- [Contexto maestro del proyecto](CONTEXTO_MAESTRO.md)
- [Hitos del proyecto](HITOS.md)
- [Lista de tareas para la próxima sesión](docs/internal/TAREAS_PENDIENTES.md)
- [Exportaciones de texto de TIA Portal](tia_portal/exports/scl/)
- [Estado de la copia de seguridad de TIA Portal](tia_portal/project_backup/README.md)

## Estructura del repositorio

```text
.
├── .gitignore
├── README.md
├── CONTEXTO_MAESTRO.md
├── HITOS.md
├── platformio.ini
├── docs/
│   ├── diagrams/
│   ├── internal/agentes/
│   └── videos/
├── include/
├── machine_simulator/
├── src/
└── tia_portal/
```

Las reglas del proyecto prohíben subir credenciales al repositorio. Copia `include/secrets.example.h` como `include/secrets.h` (archivo ignorado por Git) y guarda allí las credenciales locales. En este repositorio aún no se ha generado el archivo de archivo de TIA: un archivo `.ap21` existente no sustituye a una copia comprimida válida de Siemens en formato `.zap21`.
