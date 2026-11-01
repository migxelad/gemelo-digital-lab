# AGENTS.md — Reglas y Directivas para Agentes de IA

Este documento establece las normas obligatorias, contratos de memoria y restricciones técnicas que **CUALQUIER agente de IA** (Claude Code/Cowork/Codex, ChatGPT, Gemini Pro, etc.) debe respetar de forma estricta al generar código, proponer cambios o realizar commits en este repositorio.

---

## 1. Fuente Única de Verdad (SSOT)
* El archivo **`docs/CONTEXTO_MAESTRO.md`** es la Única Fuente de Verdad del proyecto.
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
* Las IAs de chat no modifican directamente `HITOS.md` en repositorios remotos salvo indicación expresa con herramientas Git con permisos.
* Al cerrar una sesión, el agente debe entregar una entrada formateada según el estándar definido en `HITOS.md` para ser anexada al final del archivo.
