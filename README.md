# Smart Bottling Line — Digital Twin & Hardware-in-the-Loop (HIL) Hub

## Executive summary

Digital twin for a flexible bottling cell, combining operational technology and IT: Machine Simulator Suite v5 (Nirtec), Siemens TIA Portal V21 with S7-PLCSIM in VMware, an ESP32 field node over Modbus TCP, and a planned Edge supervision layer. The repository contains the project context, PLC text exports, PlatformIO firmware, the Machine Simulator scene and engineering documentation.

## Communications topology

```text
┌──────────────────────────── Host PC ────────────────────────────┐
│ Machine Simulator Suite v5                                      │
│                 │ S7 TCP/IP                                     │
│                 ▼                                               │
│ NetToPLCsim — TCP 102, Rack 0 / Slot 1                          │
│                 │                                               │
│        VMware bridged network                                   │
│                 ▼                                               │
│ ┌────────────── VM ──────────────┐      ┌── ESP32 field node ──┐│
│ │ TIA Portal V21 + S7-PLCSIM     │      │ Modbus TCP, port 502 ││
│ └────────────────────────────────┘      └──────────────────────┘│
└─────────────────────────────────────────────────────────────────┘
                                      ··· future Edge supervision
```

Configure the VM network in bridged mode when the host, virtual PLC gateway and devices must be reachable on the same LAN. NetToPLCsim exposes the simulated S7 connection on TCP 102 with Rack 0 / Slot 1. The ESP32 serves Modbus TCP on port 502.

## Memory contract

All process digital I/O between TIA Portal and Machine Simulator / HIL starts at byte 10: `%I10.0` and `%Q10.0`. Bytes below this offset are reserved to avoid collisions with CPU/system scan memory. Do not assign process signals to `%I0.x` or `%Q0.x`. See [agent and repository rules](docs/internal/agentes/AGENTS.md).

## Process stations

- **Tank level regulation:** PID_Compact on a cyclic task is part of the proposed architecture; hardware I/O and closed-loop operation are not yet recorded as validated in this repository.
- **Infeed and buffer:** conveyor and single-container retention are defined in the master context. Verify current simulator actuators and PLC tags before implementing additional I/O.
- **Height classification:** validated on-the-fly with three discrete photocells (S/M/L) and a presence trigger. The SCL source prioritizes L → M → S and updates the active recipe without stopping the conveyor. This is discrete sensing, not analog inspection.
- **Filling and quality scale:** recipe-based filling and a 1.0 s scale settling step are planned. Weight signal scaling, target weights, PASS/REJECT sequence and hardware validation remain to be confirmed.
- **Packaging cell:** final kinematics and sequence are still an open design decision.

## Demonstrations and engineering documents

- [Video index and external viewing links](docs/videos/README.md). The source videos were removed from Git; external links and station GIF thumbnails still need to be supplied.
- [GRAFCET buffer diagram](docs/diagrams/GRAFCET_BufferStation.jpg)
- [Machine Simulator scene](machine_simulator/GemeloDigitalEnsamblado.maq)
- [Master project context](CONTEXTO_MAESTRO.md)
- [Project milestones](HITOS.md)
- [Next-session task list](docs/internal/TAREAS_PENDIENTES.md)
- [TIA Portal text exports](tia_portal/exports/scl/)
- [TIA Portal backup status](tia_portal/project_backup/README.md)

## Repository layout

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

The project rules prohibit committing credentials. Copy `include/secrets.example.h` to the ignored `include/secrets.h` and enter local credentials there. The TIA archive has not been generated in this repository: an existing `.ap21` file is not a valid substitute for a Siemens compressed `.zap21` backup.
