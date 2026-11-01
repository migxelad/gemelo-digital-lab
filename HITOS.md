Última actualización: 2026-11-01

# HITOS.md — Registro Cronológico del Proyecto

Este archivo registra el avance técnico del proyecto de forma estrictamente incremental. Las nuevas entradas se añaden siempre al final del documento.

---

## 2026-11-01 — Migración a Machine Simulator v5 y Consolidación HIL Base

- **Qué funciona ahora:**
  - Lazo de comunicación verificado entre Machine Simulator Suite v5 (Host PC) y VMware Workstation (TIA Portal V21 + S7-PLCSIM V21) mediante NetToPLCsim en puerto TCP 102.
  - Bucle Hardware-in-the-Loop (HIL) bidireccional probado: pulsador físico en protoboard con ESP32 (Modbus TCP, puerto 502) comanda la marcha de la simulación 3D; eventos virtuales conmutan el relé de 5V y LED de señalización física en mesa.
  - Lógica secuencial KOP de la Fase 1 (Buffer Station / Dosificación) validada y respaldada en vídeo demostrativo.

- **Qué cambió en el mapa de E/S o en bloques:**
  - Mapeo de memoria consolidado bajo la **Regla del Byte 10** (`%I10.0` y `%Q10.0` en adelante).
  - Variables analógicas de pesaje asignadas a `%ID100` (formato Real en kg directos).

- **Decisiones tomadas y por qué:**
  - Adopción definitiva de **Machine Simulator Suite v5** frente a Factory I/O por flexibilidad para importar modelos CAD 3D (`.obj`), edición libre de físicas y reducción de costes de licencia.
  - Distribución de agentes IA: Gemini Pro (arquitectura y master plan), Claude Code/Sonnet (desarrollo OT en SCL/Ladder), Gemini Notebook / NotebookLM (memoria grounded) y ChatGPT (sincronización auxiliar Git).

- **Pendiente / siguiente paso:**
  - Desarrollo de la **Fase 2: Estación de Inspección Híbrida** (secuenciador GRAFCET y función SCL para lectura analógica de peso `%ID100` y clasificación por matriz de altura S/M/L).

- **Archivos tocados:**
  - `docs/CONTEXTO_MAESTRO.md`
  - `AGENTS.md`
  - `HITOS.md`
  - `README.md`
