Última actualización: 2026-11-03

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

## 2026-11-03 — Validación de clasificación de altura al vuelo

- **Qué funciona ahora:** Clasificación S/M/L sin parada de cinta en Machine Simulator v5; comunicación con TIA Portal V21 / S7-PLCSIM mediante NetToPLCsim TCP 102 (Rack 0, Slot 1) en la IP validada `192.168.15.138`. La caja L conmuta `FormatID` a 3 en la Watch Table. `FC_HeightClassification` actualiza la receta activa y la conserva sin presencia.
- **Qué cambió en el mapa de E/S o en bloques:** Trigger cenital `IN_Sensor_Presencia` en `%I10.3`; fotocélulas S/M/L en `%I10.4`–`%I10.6`. OB1 acciona continuamente `Cinta1` (`%Q10.0`) y `Cinta2_DeteccionSize` (`%Q10.1`) y llama incondicionalmente a FC1. La FC prioriza L→M→S y actualiza `DB_Production.CurrentRecipe`.
- **Decisiones tomadas y por qué:** Se suprime el TON de parada en altura para evitar interrupciones mecánicas; la prueba observada en escena confirma la conmutación durante el avance. La parada de 1.0 s se reserva a la báscula para estabilizar la medida.
- **Pendiente / siguiente paso:** Crear tres contadores CTU por formato; integrar báscula con parada TON de 1.0 s; armonizar el mapa global, ya que las asignaciones anteriores de `IN_Sensor_BufferPre` (%I10.3) y `OUT_BladeStop` (%Q10.1) colisionan con las nuevas señales de altura y segunda cinta.
- **Archivos tocados:** `ContextoMaestro.md` y `HITOS.md` (documentación actualizada en este commit); proyecto TIA Portal `SortreingByWeight.zap21` (OB1, FC_HeightClassification [FC1], `typeRecipe` [UDT1]) y escena Machine Simulator v5 (pórtico y sensor cenital), según el hito aportado.

## 2026-11-03 — Versionado de FC_HeightClassification en SCL

- **Qué funciona ahora:** La lógica de clasificación S/M/L queda guardada como archivo SCL de texto en el repositorio y descrita en el contexto maestro.
- **Qué cambió en el mapa de E/S o en bloques:** Se añadió `tia_portal/exports/scl/FC_HeightClassification.scl`, con trigger `IN_Sensor_Presencia`, prioridad L→M→S y actualización de `DB_Production.CurrentRecipe`.
- **Decisiones tomadas y por qué:** El SCL se versiona por separado del contexto para facilitar revisión e importación a TIA Portal. Se normalizó `Tolereance_g` como `Tolerance_g`, nombre registrado en el UDT.
- **Pendiente / siguiente paso:** Importar/comparar el archivo con FC1 en TIA Portal y confirmar compilación; implementar los CTU de producción y la lógica de estabilización de báscula.
- **Archivos tocados:** `tia_portal/exports/scl/FC_HeightClassification.scl`, `ContextoMaestro.md`, `HITOS.md`.
