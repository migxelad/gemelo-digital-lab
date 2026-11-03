# Gemelo Digital Industrial — gemelo-digital-lab

Proyecto de célula flexible de embotellado y control metrológico para puesta en marcha virtual, con **Machine Simulator Suite v5**, **TIA Portal V21 / S7-PLCSIM** en VMware y un nodo físico **ESP32** mediante Modbus TCP.

## Estado actual

La clasificación de altura S/M/L funciona al vuelo en Machine Simulator. La tabla PLC vigente, el mapa de tags y el contexto técnico están documentados en [CONTEXTO_MAESTRO.md](CONTEXTO_MAESTRO.md). Los hitos se registran en [HITOS.md](HITOS.md). La lista priorizada de trabajo pendiente está en [TAREAS_PENDIENTES.md](TAREAS_PENDIENTES.md).

## Estructura

- `src/`, `platformio.ini`: firmware ESP32 y configuración PlatformIO.
- `tia_portal/exports/scl/`: fuentes PLC en SCL.
- `tia_portal/project_backup/`: ubicación reservada para archivos de backup comprimidos de TIA Portal.
- `machine_simulator/`: ubicación reservada para la escena nativa de Machine Simulator cuando se exporte.
- `docs/diagrams/` y `docs/videos/`: material visual del proyecto.

## Comunicaciones y reglas

- S7-PLCSIM ↔ Machine Simulator mediante NetToPLCsim, TCP 102, Rack 0 / Slot 1.
- Las E/S digitales de proceso comienzan en el Byte 10.
- ESP32 como servidor Modbus TCP, puerto 502.
- Consulta [AGENTS.md](AGENTS.md) antes de cambiar código, direcciones o arquitectura.

La escena de Machine Simulator y el backup comprimido de TIA Portal todavía no están incluidos en el repositorio; no se han creado archivos de sustitución.