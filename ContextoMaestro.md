# Gemelo Digital Industrial — Contexto Maestro del Proyecto

## 1. Resumen Ejecutivo
El proyecto comprende el desarrollo, modelado dinámico 3D, programación y validación integral de un **Gemelo Digital (Digital Twin)** para una célula flexible automatizada de envasado y control metrológico bajo una arquitectura **Hardware-in-the-Loop (HIL) bidireccional** y convergencia entre tecnologías de operaciones (OT) y tecnologías de la información (IT)[cite: 5]. El autor es Miguel Álvarez, estudiante del Grado en Ingeniería Electrónica Industrial y Automática en la Escuela de Ingeniería de Barcelona Este (EEBE), UPC – Campus Diagonal-Besòs.

El sistema conecta en bucle cerrado:
1. Planta de simulación cinemática 3D en **Machine Simulator Suite (Nirtec)** sobre el Host.
2. Autómata programable virtual industrial **Siemens SIMATIC S7-1500** ejecutado en **TIA Portal V21 / S7-PLCSIM** dentro de una máquina virtual VMware[cite: 5].
3. Hardware físico de banco de trabajo en escritorio (microcontrolador **ESP32**, botonera de mando, módulo de relés de 5V, sensor inductivo industrial M12 y motor paso a paso NEMA 17 con driver A4988) comunicado mediante **Modbus TCP** (puerto 502)[cite: 5].
4. Capa de supervisión IT / Edge proyectada (Raspberry Pi 4, contenedores Docker, base de datos relacional PostgreSQL, Node-RED, Python y panel web Streamlit)[cite: 6].

---

## 2. Objetivo del Proyecto y Resultados Esperados
* **Objetivo metodológico:** Implementar y validar la metodología industrial de **Puesta en Marcha Virtual (Virtual Commissioning)** habitual en la Industria 4.0, depurando algoritmos deterministas complejos y lazos continuos antes de la implantación física.
* **Portafolio profesional estratégico:** Construir una evidencia de ingeniería real de alto valor añadido para optar a programas de prácticas (especialmente en la división Siemens Digital Industries Software / PLM-MOM, Schneider Electric, Beckhoff, ABB u Omron) y preparar la participación en iniciativas técnicas como el hackathon *Hack the Factory*[cite: 6].
* **Incubación de TFG:** Servir como base metodológica y técnica para el Trabajo de Fin de Grado en la UPC EEBE.
* **Resultados esperados:** Célula 3D operando en tiempo real sin interferencias mecánicas; bucle HIL determinista con latencia inferior a 50 ms; arquitectura de bloques modular conforme a IEC 61131-3 (OB, FB, FC, DB, UDT); y repositorio Git estructurado (`gemelo-digital-lab`).

---

## 3. Concepto del Proceso y Evolución del Diseño
* **Fase Inicial (Prototipo Logístico en Factory I/O — Descartada/Sustituida):**
  * *Diseño original:* Línea logística de transporte y clasificación de cajas con 5 subestaciones (Buffer, pesaje y desvíos paralelos por altura con cilindros neumáticos)[cite: 5, 8].
  * *Causas del abandono:* Sobrecarga del motor físico PhysX de Factory I/O al ejecutarse en paralelo con VMware en un equipo con 16 GB de RAM física, desplomando la tasa de refresco por debajo de 25 FPS ("efecto Frankenstein")[cite: 5, 9]. Adicionalmente, el modelo de suscripción mensual recurrente (28 €/mes) y la imposibilidad de importar modelos CAD externos `.obj` limitaban la flexibilidad del proyecto[cite: 5, 7].
* **Fase Vigente (Célula Flexible de Embotellado en Machine Simulator Suite — Confirmada):**
  * *Motivación:* Adquisición de licencia anual perpetua de Nirtec (92 €), libertad total de cinemática de cuerpos rígidos, compatibilidad nativa con modelos CAD `.obj` (botellas, boquillas, grippers) y drivers directos S7 TCP/IP y Modbus TCP[cite: 5, 7].
  * *Concepto:* Proceso híbrido de alta densidad lógica que acopla un lazo de regulación continua (nivel de líquido mediante PID) con manufactura discreta secuencial (dosificación, clasificación dimensional, llenado por receta, auditoría dinámica en báscula, taponado por cobot y packaging final)[cite: 7].

---

## 4. Diseño Actual de la Línea y Estaciones
1. **Estación 0 — Regulación Continua de Nivel de Tanque (Lazo PID):**
   * Depósito de almacenamiento con transmisor analógico de nivel (0-10V, 0-27648 en PLC).
   * Bloque tecnológico `PID_Compact` en TIA Portal (OB30 cíclico a 100 ms) modulando la válvula proporcional o bomba de entrada.
   * Interlock crítico: La línea no arranca si el nivel no alcanza el Setpoint definido. Alarma y parada de seguridad si el nivel desciende por debajo del umbral crítico (< 15%).
2. **Estación 1 — Dosificación y Retención de Entrada:**
   * Cinta de alimentación gobernada por motor digital (`OUT_Conveyor_Entry`).
   * Retención unitaria mediante cilindro neumático vertical (*Blade Stop*, `OUT_BladeStop`) supervisado por fotocélula de detección previa (`IN_Sensor_BufferPre`)[cite: 11].
3. **Estación 2 — Clasificación Dimensional de Altura y Recetas:**
   * Columna vertical de fotocélulas que discrimina el formato de envase antes de la boquilla:
     * Formato Small (S): Sensor inferior activo (`IN_Sensor_Height_S`). Consigna de llenado: 25 cl (250 ml), tiempo: 1.5 s, tolerancia de masa: Nominal S ± 8 g.
     * Formato Medium (M): Sensores inferior y medio activos (`IN_Sensor_Height_M`). Consigna: 50 cl (500 ml), tiempo: 3.0 s, tolerancia de masa: Nominal M ± 12 g.
     * Formato Large (L): Sensores inferior, medio y superior activos (`IN_Sensor_Height_L`). Consigna: 75/100 cl, tiempo: 4.5 s, tolerancia de masa: Nominal L ± 18 g.
   * Generación de estructura de datos de receta (UDT) en SCL.
4. **Estación 3 — Dosificación / Llenado Vertical:**
   * Retenedor neumático de centrado (`IN_Sensor_FillPos`).
   * Actuador neumático vertical de 2 ejes que desciende la boquilla (`OUT_Nozzle_Down`) y electroválvula de apertura de fluido (`OUT_Valve_Liquid`) temporizada con el parámetro exacto de la receta.
5. **Estación 4 — Auditoría Metrológica (Báscula Dinámica) y Descarte Neumático:**
   * Cinta transportadora montada sobre célula de carga (*Weight System*, `OUT_Conveyor_Scale`)[cite: 7].
   * Presencia detectada por fotocélula (`IN_Sensor_ScaleStop`), parada de cinta y temporizador TON de 1.0 s para estabilización mecánica de la báscula[cite: 7].
   * Evaluación en SCL: Si masa real está dentro de rango (PASS), avanza a taponado. Si la masa está fuera de rango (REJECT), el empujador neumático lateral (`OUT_Pusher_Reject`) se dispara a 90° expulsando la botella defectuosa a una rampa con bloque destructor de piezas.
6. **Estación 5 — Taponado por Cobot (Cobot Capping):**
   * Robot colaborativo que toma un tapón preformado, desciende concéntricamente sobre el cuello, aplica par de apriete/presión y retorna a cota segura, emitiendo pulso booleano `IN_Cobot_Done` hacia el PLC (sugerencia técnica incorporada del profesor Pere Ponsa).
7. **Estación 6 — Célula de Packaging Final (Dilema Cinemático):**
   * Mesa de acumulación final evaluando dos alternativas:
     * Opción A: Pórtico cartesiano XYZ controlado mediante cinemática directa y matrices en SCL.
     * Opción B: Robot industrial articulado de 6 ejes con garra terminal de vacío (ventosas) y giro de 180° para encajonado.

---

## 5. Estado de Montaje en Machine Simulator
* **Plataforma:** Machine Simulator Suite (Nirtec) v5 ejecutándose nativamente sobre el Host de Windows para aprovechar aceleración física y gráfica por GPU.
* **Componentes ensamblados:** Estructura completa de cintas modulares, depósito de líquido con indicador visual, cilindros neumáticos (*Blade Stop* y empujador lateral), báscula *Weight System*, boquilla dosificadora, sensor inductivo virtual, columna de fotocélulas, rampa de descarte, bloque *WorkPart Destroyer* y *WorkPart Creator*[cite: 7].
* **Generación de piezas:** Módulo *WorkPart Creator* configurado para instanciar botellas de formatos S, M y L en secuencia o aleatoriamente, incorporando defectos programados de masa o tara para verificar el rechazo de scrap.

---

## 6. Arquitectura de Control, Comunicaciones y Guías Operativas

### 6.1. Diagrama de Enlace y Parámetros de Red