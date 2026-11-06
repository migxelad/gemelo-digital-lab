# docs/internal/agentes/AGENTS.md — Reglas y Directivas para Agentes de IA

Este documento establece las normas obligatorias, contratos de memoria y restricciones técnicas que **CUALQUIER agente de IA** (Claude Code/Cowork/Codex, ChatGPT, Gemini Pro, etc.) debe respetar de forma estricta al generar código, proponer cambios o realizar commits en este repositorio.

---

## 1. Fuente Única de Verdad (SSOT)
* El archivo **`CONTEXTO_MAESTRO.md`** es la Única Fuente de Verdad del proyecto.
* Cualquier modificación en la arquitectura, mapa de memoria o secuencias debe quedar reflejada en el contexto maestro y registrarse en `HITOS.md`.

---

## 2. Contrato Inquebrantable de Memoria (Regla del Byte 10)
* **Offset en Byte 10:** Todas las entradas y salidas digitales entre TIA Portal y Machine Simulator / hardware HIL **deben desplazarse obligatoriamente a partir del Byte 10** (`%I10.0` y `%Q10.0` en adelante).
* **Motivo:** Previene que el escaneo interno de la CPU SIMATIC o las marcas de ciclo/sistema (%MB0..%MB9) sobrescriban las variables de proceso. Queda prohibido asignar E/S de proceso en `%I0.x` o `%Q0.x`.

---

## 3. Configuración de Hardware y CPU Siemens (TIA Portal V21)
Al generar o modificar configuraciones para la CPU SIMATIC S7-1500 (ej. S7-1511-1 PN):
1. **PUT/GET:** Debe estar activa la casilla *"Permit access with PUT/GET communication from remote partner"*.
2. **Acceso:** Nivel de protección configurado en *"Full access (no protection)"*.
3. **Simulación:** Habilitada la opción *"Support simulation during block compilation"*.
4. **Bloques de Datos (DB):** Todos los DBs expuestos a comunicación externa o pasarelas **deben tener desmarcado "Optimized Block Access"**.

---

## 4. Red, Puertos y Comunicaciones HIL
* **NetToPLCsim:** Opera en el puerto TCP **102** redirigiendo el tráfico hacia S7-PLCSIM (Rack 0, Slot 1 obligatorio para S7-1500).
* **ESP32 (Nodo de campo HIL):** Servidor Modbus TCP en el puerto **502**.
* **Simulador 3D:** Machine Simulator Suite v5.

---

## 5. Estándares de Lenguaje y Programación
* **Lógica Secuencial y Modos de Marcha:** Programados en **Ladder / KOP** bajo norma IEC 61131-3 (GRAFCET).
* **Tratamiento Analógico, Recetas y Calidad:** Programados estrictamente en **Texto Estructurado (SCL)** (escalado, comparaciones de peso/altura, registros FIFO).
* **Firmware ESP32:** C++ bajo PlatformIO / Arduino framework sin bloques retardantes (`delay()`).

---

## 6. Prohibición de Binarios Pesados en Git
* **No subir proyectos TIA binarios pesados uncompressed** (`.ap21`, `.zap21` pesados) directamente a la raíz.
* El código de control debe versionarse en **texto plano (`.scl`)** o exportaciones de **tablas de tags (`.xml`)**.
* Los respaldos binarios comprimidos de TIA Portal deben guardarse en la carpeta designada `tia_portal/project_backup/`.

---

## 7. Protocolo de Registro de Hitos
* Solo el agente con rol de **Inspector** (ver sección 8) anexa entradas a `HITOS.md`, y únicamente después de validar el cambio.
* Las entradas se añaden siempre al **final** del archivo y se actualiza la primera línea `Última actualización: AAAA-MM-DD`.
* Las IAs de chat sin herramientas Git (Gemini, ChatGPT, NotebookLM) no modifican el repositorio: entregan la entrada formateada para que la anexe el Inspector o Miguel.

---

## 8. Roles de Agentes y Reglas Anti-Solapamiento

### 8.1 Principio rector
Quien escribe el código no lo aprueba. Todo cambio pasa por: **Constructor → Inspector → Miguel (merge)**.

### 8.2 Reparto de roles
| Agente | Rol | Territorio |
|---|---|---|
| **Codex** | Constructor | Escribe código nuevo y refactorizaciones: bloques SCL (`tia_portal/exports/scl/`), tablas de tags XML, firmware ESP32 (`src/`, `platformio.ini`) y, en Fase 3, Node-RED, Docker, SQL y Python. Mantenimiento estructural del repo (carpetas, `.gitignore`, README). |
| **Claude (Code / Cowork)** | Inspector y jefe de obra | Revisa cada PR contra este `docs/internal/agentes/AGENTS.md` y `CONTEXTO_MAESTRO.md`. Verifica en la simulación real (TIA Portal, Watch Table, Machine Simulator) mediante control del PC. Es el **único agente que añade entradas a `HITOS.md`**, y solo tras validar. Mantiene el estado de `docs/internal/TAREAS_PENDIENTES.md`. |
| **Miguel** | Ingeniero responsable | Decide arquitectura, opera TIA Portal cuando hace falta, resuelve decisiones abiertas y es el **único que fusiona en `main`**. |

### 8.3 Reglas obligatorias
1. **Etiqueta de dueño:** cada tarea de `docs/internal/TAREAS_PENDIENTES.md` lleva una etiqueta `[codex]`, `[claude]` o `[miguel]` (pueden combinarse, p. ej. `[codex→claude]` = Codex implementa, Claude valida). Ningún agente ejecuta una tarea que no lleve su etiqueta.
2. **Ramas por agente:** Codex trabaja en `codex/<tema>`, Claude en `claude/<tema>`. Prohibido el push directo a `main`.
3. **Un tema, una rama, un agente:** si una rama de un agente está abierta sobre un tema, el otro no abre otra rama sobre ese mismo tema.
4. **Entrega de Codex:** cada PR incluye en su descripción (a) qué tarea de `docs/internal/TAREAS_PENDIENTES.md` cierra, (b) qué reglas de `docs/internal/agentes/AGENTS.md` afecta, y (c) un **borrador** de entrada para `HITOS.md` con el formato estándar. Codex **no** edita `HITOS.md`.
5. **Revisión de Claude:** Claude deja su revisión en `docs/reviews/AAAA-MM-DD_<tema>.md` (veredicto APROBADO / CAMBIOS, hallazgos con archivo y línea, y resultado de la validación en simulación si la hubo). Tras validar, anexa la entrada definitiva al final de `HITOS.md` y marca la tarea como `[x]`.
6. **Conflicto o duda:** si un agente detecta que necesita tocar el territorio del otro, lo anota en la descripción del PR o en la revisión y se detiene; no lo resuelve por su cuenta.
7. **Cambio de reglas:** solo Miguel modifica esta sección 8.
