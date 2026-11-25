# Gemelo digital de una línea de embotellado

Proyecto personal de automatización que integra Machine Simulator Suite v5, TIA Portal V21 con S7-PLCSIM y un nodo ESP32 mediante Modbus TCP.  
Es un trabajo en curso: este repositorio recoge el estado actual y se actualizará con cada avance. Consulta el [registro de hitos](HITOS.md).

## Estado del proyecto

| Componente | Estado | Evidencia |
|---|---|---|
| Comunicación Machine Simulator ↔ NetToPLCsim ↔ S7-PLCSIM | Validado | [Hito de integración HIL](HITOS.md#2026-11-01--migración-a-machine-simulator-v5-y-consolidación-hil-base) |
| Bucle HIL base con pulsador, relé y LED mediante ESP32 | Validado | [Hito HIL](HITOS.md#2026-11-01--migración-a-machine-simulator-v5-y-consolidación-hil-base); [firmware](src/main.cpp) |
| Clasificación de altura S/M/L al vuelo | Validado | [Hito de clasificación](HITOS.md#2026-11-03--validación-de-clasificación-de-altura-al-vuelo); [FC_HeightClassification](tia_portal/exports/scl/FC_HeightClassification.scl) |
| Contadores por formato | Planificado | Pendiente de implementación y validación; [tareas](docs/internal/TAREAS_PENDIENTES.md#próxima-sesión--cerrar-contadores-de-clasificación) |
| Báscula y secuencia PASS/REJECT | Planificado | Tags, escala y secuencia por confirmar; [tareas](docs/internal/TAREAS_PENDIENTES.md#báscula-dinámica-y-descarte) |
| PID_Compact para nivel del tanque en OB cíclico | Planificado | Propuesto en el contexto; implementación y validación pendientes |
| Seguimiento de envases con array de UDT | Planificado | Pendiente de diseñar y confirmar |
| Taponado con cobot | Planificado | Estación prevista en el concepto; integración pendiente |
| Packaging final (Gantry XYZ o robot de seis ejes) | Planificado | Decisión de cinemática pendiente; [tareas](docs/internal/TAREAS_PENDIENTES.md#decisiones-todavía-abiertas) |
| Supervisión Edge (Docker, PostgreSQL y MQTT) | Planificado | Capa proyectada; aún no implementada |
| Sensor inductivo y motor NEMA 17 en firmware | Planificado | El código actual solo define pulsador, relé y LED; la ampliación está pendiente |
| Prueba con un SIMATIC físico | Planificado | Pendiente de planificar; [tareas](docs/internal/TAREAS_PENDIENTES.md#decisiones-todavía-abiertas) |

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

NetToPLCsim expone la conexión S7 simulada en TCP 102 con Rack 0 / Slot 1. El ESP32 ofrece Modbus TCP en el puerto 502.

## Contrato de memoria

Todas las E/S digitales de proceso entre TIA Portal y Machine Simulator / HIL comienzan en el byte 10: `%I10.0` y `%Q10.0`. No asignes señales de proceso a `%I0.x` ni `%Q0.x`. Consulta las [reglas del repositorio](docs/internal/agentes/AGENTS.md).

## Próximos pasos

- Confirmar en TIA Portal los contadores por formato y completar su implementación y validación.
- Verificar los actuadores del cabezal y el retenedor en Machine Simulator y TIA Portal.
- Confirmar señal, escala, unidad, tara y valores objetivo de la báscula antes de implementar PASS/REJECT.
- Revisar el mapa vigente de tags y resolver las colisiones históricas indicadas en la bitácora.
- Decidir la cinemática de packaging y planificar una prueba con un S7-1500 físico.

## Demostraciones y documentación

- [Vídeos y enlaces de visualización](docs/videos/README.md)
- [Diagrama GRAFCET de la estación buffer](docs/diagrams/GRAFCET_BufferStation.jpg)
- [Escena de Machine Simulator](machine_simulator/GemeloDigitalEnsamblado.maq)
- [Contexto maestro](CONTEXTO_MAESTRO.md)
- [Hitos del proyecto](HITOS.md)
- [Tareas pendientes](docs/internal/TAREAS_PENDIENTES.md)
- [Exportaciones SCL de TIA Portal](tia_portal/exports/scl/)
- [Estado de la copia de seguridad de TIA Portal](tia_portal/project_backup/README.md)

El archivo comprimido oficial de TIA Portal en formato `.zap21` está pendiente de subir; consulta el [estado del backup](tia_portal/project_backup/README.md).

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
│   ├── internal/
│   └── videos/
├── include/
├── machine_simulator/
├── src/
└── tia_portal/
```

## Metodología y herramientas

El autor es responsable del diseño, las decisiones técnicas y la validación en simulación. Se emplean asistentes de IA como apoyo para la documentación y la revisión de código.
