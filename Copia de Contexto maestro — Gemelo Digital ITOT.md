# **Gemelo Digital Industrial — Contexto Maestro del Proyecto**

# **1\. Resumen Ejecutivo**

El proyecto comprende el desarrollo, modelado dinámico 3D, programación y validación integral de un **Gemelo Digital (Digital Twin)** para una célula flexible automatizada de envasado y control metrológico bajo una arquitectura **Hardware-in-the-Loop (HIL) bidireccional** y convergencia entre tecnologías de operaciones (OT) y tecnologías de la información (IT). El autor es Miguel Álvarez, estudiante del Grado en Ingeniería Electrónica Industrial y Automática en la Escuela de Ingeniería de Barcelona Este (EEBE), UPC – Campus Diagonal-Besòs.

El sistema conecta en bucle cerrado:

1. Planta de simulación cinemática 3D en **Machine Simulator Suite (Nirtec)** sobre el Host.  
2. Autómata programable virtual industrial **Siemens SIMATIC S7-1500** ejecutado en **TIA Portal V21 / S7-PLCSIM** dentro de una máquina virtual VMware.  
3. Hardware físico de banco de trabajo en escritorio (microcontrolador **ESP32**, botonera de mando, módulo de relés de 5V, sensor inductivo industrial M12 y motor paso a paso NEMA 17 con driver A4988) comunicado mediante **Modbus TCP** (puerto 502).  
4. Capa de supervisión IT / Edge proyectada (Raspberry Pi 4, contenedores Docker, base de datos relacional PostgreSQL, Node-RED, Python y panel web Streamlit).

# **2\. Objetivo del Proyecto y Resultados Esperados**

* **Objetivo metodológico:** Implementar y validar la metodología industrial de **Puesta en Marcha Virtual (Virtual Commissioning)** habitual en la Industria 4.0, depurando algoritmos deterministas complejos y lazos continuos antes de la implantación física.  
* **Portafolio profesional estratégico:** Construir una evidencia de ingeniería real de alto valor añadido para optar a programas de prácticas (especialmente en la división Siemens Digital Industries Software / PLM-MOM, Schneider Electric, Beckhoff, ABB u Omron) y preparar la participación en iniciativas técnicas como el hackathon *Hack the Factory*.  
* **Incubación de TFG:** Servir como base metodológica y técnica para el Trabajo de Fin de Grado en la UPC EEBE.  
* **Resultados esperados:** Célula 3D operando en tiempo real sin interferencias mecánicas; bucle HIL determinista con latencia inferior a 50 ms; arquitectura de bloques modular conforme a IEC 61131-3 (OB, FB, FC, DB, UDT); y repositorio Git estructurado (`gemelo-digital-lab`).

# **3\. Concepto del Proceso y Evolución del Diseño**

* **Fase Inicial (Prototipo Logístico en Factory I/O — Descartada/Sustituida):**  
  * *Diseño original:* Línea logística de transporte y clasificación de cajas con 5 subestaciones (Buffer, pesaje y desvíos paralelos por altura con cilindros neumáticos).  
  * *Causas del abandono:* Sobrecarga del motor físico PhysX de Factory I/O al ejecutarse en paralelo con VMware en un equipo con 16 GB de RAM física, desplomando la tasa de refresco por debajo de 25 FPS ("efecto Frankenstein"). Adicionalmente, el modelo de suscripción mensual recurrente (28 €/mes) y la imposibilidad de importar modelos CAD externos `.obj` limitaban la flexibilidad del proyecto.  
* **Fase Vigente (Célula Flexible de Embotellado en Machine Simulator Suite — Confirmada):**  
  * *Motivación:* Adquisición de licencia anual perpetua de Nirtec (92 €), libertad total de cinemática de cuerpos rígidos, compatibilidad nativa con modelos CAD `.obj` (botellas, boquillas, grippers) y drivers directos S7 TCP/IP y Modbus TCP.  
  * *Concepto:* Proceso híbrido de alta densidad lógica que acopla un lazo de regulación continua (nivel de líquido mediante PID) con manufactura discreta secuencial (dosificación, clasificación dimensional, llenado por receta, auditoría dinámica en báscula, taponado por cobot y packaging final).

# **4\. Diseño Actual de la Línea y Estaciones**

1. **Estación 0 — Regulación Continua de Nivel de Tanque (Lazo PID):**  
   * Depósito de almacenamiento con transmisor analógico de nivel (0-10V, 0-27648 en PLC).  
   * Bloque tecnológico `PID_Compact` en TIA Portal (OB30 cíclico a 100 ms) modulando la válvula proporcional o bomba de entrada.  
   * Interlock crítico: La línea no arranca si el nivel no alcanza el Setpoint definido. Alarma y parada de seguridad si el nivel desciende por debajo del umbral crítico (\< 15%).  
2. **Estación 1 — Dosificación y Retención de Entrada:**  
   * Cinta de alimentación gobernada por motor digital (`OUT_Conveyor_Entry`).  
   * Retención unitaria mediante cilindro neumático vertical (*Blade Stop*, `OUT_BladeStop`) supervisado por fotocélula de detección previa (`IN_Sensor_BufferPre`).  
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
   * Cinta transportadora montada sobre célula de carga (*Weight System*, `OUT_Conveyor_Scale`).  
   * Presencia detectada por fotocélula (`IN_Sensor_ScaleStop`), parada de cinta y temporizador TON de 1.0 s para estabilización mecánica de la báscula.  
   * Evaluación en SCL: Si masa real está dentro de rango (PASS), avanza a taponado. Si la masa está fuera de rango (REJECT), el empujador neumático lateral (`OUT_Pusher_Reject`) se dispara a 90° expulsando la botella defectuosa a una rampa con bloque destructor de piezas.  
6. **Estación 5 — Taponado por Cobot (Cobot Capping):**  
   * Robot colaborativo que toma un tapón preformado, desciende concéntricamente sobre el cuello, aplica par de apriete/presión y retorna a cota segura, emitiendo pulso booleano `IN_Cobot_Done` hacia el PLC. (Sugerencia técnica incorporada del profesor Pere Ponsa).  
7. **Estación 6 — Célula de Packaging Final (Dilema Cinemático):**  
   * Mesa de acumulación final evaluando dos alternativas:  
     * Opción A: Pórtico cartesiano XYZ controlado mediante cinemática directa y matrices en SCL.  
     * Opción B: Robot industrial articulado de 6 ejes con garra terminal de vacío (ventosas) y giro de 180° para encajonado.

# **5\. Estado de Montaje en Machine Simulator**

* **Plataforma:** Machine Simulator Suite (Nirtec) v5 ejecutándose nativamente sobre el Host de Windows para aprovechar aceleración física y gráfica por GPU.  
* **Componentes ensamblados:** Estructura completa de cintas modulares, depósito de líquido con indicador visual, cilindros neumáticos (*Blade Stop* y empujador lateral), báscula *Weight System*, boquilla dosificadora, sensor inductivo virtual, columna de fotocélulas, rampa de descarte, bloque *WorkPart Destroyer* y *WorkPart Creator*.  
* **Generación de piezas:** Módulo *WorkPart Creator* configurado para instanciar botellas de formatos S, M y L en secuencia o aleatoriamente, incorporando defectos programados de masa o tara para verificar el rechazo de scrap.

# **6\. Arquitectura de Control, Comunicaciones y Red**

* **Host (Windows):** Machine Simulator Suite conectado mediante drivers nativos:  
  * Driver Siemens S7 TCP/IP hacia la IP de la máquina virtual (puerto TCP 102).  
  * Driver Modbus TCP hacia la IP del microcontrolador ESP32 (puerto TCP 502).  
* **Máquina Virtual (VMware Workstation):**  
  * Sistema operativo: Windows 10 con adaptador de red en modo **Bridged** (en la misma subred física del host y router local). Firewall desactivado.  
  * TIA Portal V21 \+ S7-PLCSIM emulando una CPU SIMATIC S7-1511-1 PN.  
  * Pasarela **NetToPLCsim:** Ejecutada como Administrador tras detener el servicio de Windows `SIMATIC S7DOS Help Service`. Configuración: *Network IP* \= IP asignada a la VM (ej. 192.168.15.131); *PLCSIM IP* \= 192.168.0.1; *Rack* \= 0; *Slot* \= 1 (obligatorio para familias S7-1500). Estado operativo: `RUNNING`.  
* **Ajustes críticos en CPU SIMATIC:**  
  * Habilitar casilla "Support simulation during block compilation".  
  * Habilitar casilla "Permit access with PUT/GET communication from remote partner".  
  * Nivel de protección: "Full access (no protection)".  
  * Desactivar la opción "Optimized block access" en los DBs expuestos a lectura externa.  
* **Regla inquebrantable de memoria (Contrato del Byte 10):**  
  * Para evitar que el barrido cíclico de la CPU o los bytes de sistema (%MB0) sobrescriban con ceros la memoria de proceso en PLCSIM, **todas las E/S de proceso se asignan estrictamente a partir del Byte 10 (%I10.0 y %Q10.0 en adelante)**.

# **7\. Mapa de Entradas y Salidas**

## **Entradas Digitales (PLC Inputs)**

* `%I10.0` \- `IN_HIL_BtnStart` (BOOL): Pulsador físico real en protoboard \[Confirmado\].  
* `%I10.1` \- `IN_HIL_BtnStop` (BOOL): Pulsador físico real de parada/rearme \[Confirmado\].  
* `%I10.2` \- `IN_HIL_SensorFault` (BOOL): Sensor inductivo físico (inyección de fallos) \[Confirmado\].  
* `%I10.3` \- `IN_Sensor_BufferPre` (BOOL): Fotocélula previa a tope dosificador \[Confirmado\].  
* `%I10.4` \- `IN_Sensor_Height_S` (BOOL): Fotocélula inferior cota formato S \[Confirmado\].  
* `%I10.5` \- `IN_Sensor_Height_M` (BOOL): Fotocélula intermedia cota formato M \[Confirmado\].  
* `%I10.6` \- `IN_Sensor_Height_L` (BOOL): Fotocélula superior cota formato L \[Confirmado\].  
* `%I10.7` \- `IN_Sensor_FillPos` (BOOL): Detección de botella centrada en boquilla \[Propuesta técnica\].  
* `%I11.0` \- `IN_Sensor_ScaleStop` (BOOL): Fotocélula de centrado en báscula \[Confirmado\].  
* `%I11.1` \- `IN_Cobot_Done` (BOOL): Señal de fin de ciclo de taponado \[Confirmado\].  
* `%I11.2` \- `IN_Sensor_ExitDetect` (BOOL): Fotocélula de evacuación final \[Confirmado\].

## **Salidas Digitales (PLC Outputs)**

* `%Q10.0` \- `OUT_Conveyor_Entry` (BOOL): Motor cinta de entrada / Buffer \[Confirmado\].  
* `%Q10.1` \- `OUT_BladeStop` (BOOL): Accionamiento tope neumático vertical \[Confirmado\].  
* `%Q10.2` \- `OUT_Nozzle_Down` (BOOL): Descenso vertical boquilla de dosificación \[Confirmado\].  
* `%Q10.3` \- `OUT_Valve_Liquid` (BOOL): Electroválvula de llenado de líquido \[Confirmado\].  
* `%Q10.4` \- `OUT_Conveyor_Scale` (BOOL): Motor de cinta sobre báscula \[Confirmado\].  
* `%Q10.5` \- `OUT_Pusher_Reject` (BOOL): Empujador neumático descarte a 90° \[Confirmado\].  
* `%Q10.6` \- `OUT_Cobot_Start` (BOOL): Disparo de ciclo de cierre/roscado del cobot \[Confirmado\].  
* `%Q10.7` \- `OUT_HIL_RelayAlarm` (BOOL): Relé 1 / Maniobra física de alarma o rechazo \[Confirmado\].  
* `%Q11.0` \- `OUT_HIL_RelayCycle` (BOOL): Relé 2 / Confirmación física de ciclo en marcha \[Confirmado\].

## **Registros Analógicos y Posicionamiento**

* `%IW64` \- `ANA_Tank_Level` (INT, 0-27648): Nivel transmisor depósito (0.0 \- 100.0 %) \[Confirmado\].  
* `%QW64` \- `CMD_Pump_Inflow` (INT, 0-27648): Salida analógica PID bomba/válvula \[Confirmado\].  
* `%ID100` o `%IW66` \- `ANA_Scale_Weight` (REAL/INT): Medida de masa en báscula (0.0 \- 2000.0 g) \[Confirmado\].  
* `%QW66` \- `HIL_NEMA17_Steps` (INT): Consigna angular para motor paso a paso espejo \[Validado en Modbus\].

# **8\. Secuencias de Funcionamiento y Lógica PLC**

* **Estructura en TIA Portal:**  
  * `OB1`: Bucle determinista maestro, modos de marcha, llamadas a secuenciadores y gestión de interlocks.  
  * `OB30` (Interrupción cíclica a 100 ms): Ejecución dedicada del lazo `PID_Compact` de nivel de líquido.  
  * `FB_Buffer_Manager` (Ladder / KOP): Máquina de estados (GRAFCET) para retención y suelta unitaria de piezas.  
  * `FB_Quality_Engine` / `FC_Analog_Scaling` (SCL): Tratamiento analógico con `NORM_X`/`SCALE_X`, evaluación matricial de tolerancias y asignación de receta.  
  * `DB_Tracking_FIFO`: Registro dinámico para correlacionar el envase medido con la botella que entra a la báscula.

# **9\. Hardware Físico e Integración HIL**

* **Microcontrolador ESP32 (WROOM-32):** Nodo esclavo Modbus TCP sobre Wi-Fi local (puerto 502\) con temporizador *Watchdog/Keep-Alive* (1500 ms).  
* **Módulo de Relés 5V (4 canales optoacoplados):** Maniobra de conmutación audible ante descartes y señalización física, alimentado en riel externo independiente para prevenir *brownouts*.  
* **Instrumentación de campo:** Pulsadores cableados en pull-up interno hacia masa; sensor inductivo industrial tubular con divisor resistivo de tensión para adaptar señal de 12/24V a 3.3V GPIO.  
* **Cinemática espejo:** Motor bipolar NEMA 17 gobernado por driver A4988 (pines Step/Dir en GPIOs 25, 26 y 27\) girando en sincronía con el proceso virtual.  
* **Infraestructura de taller auxiliar:** Osciloscopio/multímetro FNIRSI 2C53T (3 en 1 portátil), fuente lineal regulable 30V/5A, estación de soldadura rápida T12.

# **10\. Decisiones Vigentes y Motivos**

1. Selección de Machine Simulator Suite: Solución de licencia fija (92 €), importación CAD de envases reales y drivers nativos sin sobrecarga gráfica.  
2. Contrato del Byte 10: Blindaje de memoria frente a la sobreescritura interna de Siemens PLCSIM.  
3. Proceso híbrido continuo/discreto: Inclusión del lazo PID de nivel para maximizar el rigor técnico y la densidad lógica del proyecto.  
4. Repositorio centralizado en GitHub (`gemelo-digital-lab`): Trazabilidad integral de código C++, exportaciones SCL, documentación y esquemas eléctricos.

# **11\. Propuestas Descartadas o Sustituidas y Motivos**

* Maqueta extensa de 5 ramales en Factory I/O: Descartada por saturación física (caída de FPS \< 25\) e inviabilidad de coste por suscripción recurrente.  
* Evasión de licencias Trial Siemens mediante snapshots de VMware: Descartada por provocar inconsistencias en el *Automation License Manager* (ALM). Los snapshots quedan reservados para recuperación ante desastres (*Disaster Recovery*).

# **12\. Problemas Encontrados y Soluciones Probadas**

1. Bloqueo del puerto TCP 102 en la VM: Conflicto con el servicio `SIMATIC S7DOS Help Service`. Solución: detención previa del servicio y ejecución de NetToPLCsim como Administrador.  
2. Rechazo de conexión en NetToPLCsim por Slot incorrecto: Denegado al configurar Slot 2\. Solución: fijar Slot 1 correspondiente a la CPU S7-1500.  
3. Pérdida de comunicación VM-Host: Fallos en adaptador NAT. Solución: configurar adaptador de VMware en modo Bridged sobre la interfaz de red física activa.  
4. Valores analógicos forzados a cero: Solucionado mediante el desplazamiento de offset al Byte 10\.

# **13\. Dudas y Tareas Pendientes**

* Resolución del dilema cinemático de packaging: Selección final entre pórtico cartesiano XYZ (matrices en SCL) y brazo robótico de 6 ejes con garra de vacío.  
* Despliegue de los bloques SCL de la Fase 2: Implementación de la función `FC_Analog_Scaling` y estructura UDT de recetas.  
* Pruebas institucionales en UPC EEBE: Coordinación con el departamento para transferir el proyecto a un autómata SIMATIC S7-1500 real.

# **14\. Riesgos Técnicos y Pruebas Necesarias**

* Determinismo de red: Comprobación de que la latencia acumulada entre Machine Simulator, NetToPLCsim y el ESP32 no exceda los 50 ms.  
* Saturación de recursos de cómputo: Monitoreo de memoria RAM y núcleos de CPU durante la ejecución simultánea de VMware y Machine Simulator a 60 Hz.

# **15\. Cronología de Avances**

* *Septiembre 2026:* Definición de arquitectura base IT/OT y adquisición escalonada de componentes de instrumentación.  
* *Octubre 2026 (Semana 1):* Configuración de la máquina virtual VMware, pasarela NetToPLCsim en puerto 102 y primer lazo cerrado HIL operativo.  
* *Octubre 2026 (Semana 2):* Depuración de la estación Buffer en Ladder y decisión de migración a Machine Simulator.  
* *Octubre 2026 (Semana 3):* Rediseño hacia la célula flexible de embotellado con lazo PID y redacción de propuesta para TFG en la UPC EEBE.  
* *Octubre 2026 (Semana 4):* Escena 3D ensamblada en Machine Simulator, verificación del enlace S7 Ethernet hacia PLCSIM V21 y preparación de bloques en SCL.

# **16\. Fuentes Revisadas**

1. Historial del cuaderno de trabajo y sesiones de diseño metodológico IT/OT.  
2. Documento de propuesta técnica de célula de embotellado (`Propuesta Gemelo Digital Embotellado (Machine Simulator + TIA Portal)_MiguelDanielAlvarez.gdoc` y `.pdf`).  
3. Formulación técnica académica inicial para la UPC EEBE (`Propuesta Técnica - Gemelo Digital HIL IT-OT (UPC EEBE).gdoc`).  
4. Planificación horaria (`Horario_Productivo_Uni_ITOT.gsheet`).

