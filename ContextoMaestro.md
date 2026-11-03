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
   * Clasificación validada "al vuelo" en Machine Simulator v5 mediante tres fotocélulas discretas independientes y una fotocélula cenital de trigger:
     * `%I10.3` — `IN_Sensor_Presencia`: detección cenital de centrado; habilita la lectura en el plano de medida.
     * `%I10.4` — `IN_Sensor_Height_S`: cota baja.
     * `%I10.5` — `IN_Sensor_Height_M`: cota intermedia.
     * `%I10.6` — `IN_Sensor_Height_L`: cota alta.
   * `FC_HeightClassification` [FC1], en SCL, evalúa solo cuando `IN_Sensor_Presencia` está activo y prioriza las cotas de mayor a menor (L → M → S). Actualiza `DB_Production.CurrentRecipe` (`typeRecipe`) y conserva la receta anterior cuando no hay presencia:
     * L: `FormatID := 3`, `TargetVolume_cl := 75.0`, `FillTime := T#4S_500MS`, `Tolerance_g := 18.0`.
     * M: `FormatID := 2`, `TargetVolume_cl := 50.0`, `FillTime := T#3S`, `Tolerance_g := 12.0`.
     * S: `FormatID := 1`, `TargetVolume_cl := 25.0`, `FillTime := T#1S_500MS`, `Tolerance_g := 8.0`.
   * Validación empírica: el `FormatID` conmuta a 3 al pasar una caja L sin detener la cinta; se observó en la Watch Table con PLCSIM V21 y NetToPLCsim.
   * En esta estación no se usa TON de parada. La parada de 1.0 s queda reservada a la báscula de la Estación 4.
   * **Pendiente de armonización del mapa global:** la documentación anterior asignaba `%I10.3` a `IN_Sensor_BufferPre` y `%Q10.1` a `OUT_BladeStop`. La configuración validada de esta estación usa `%I10.3` como trigger y `%Q10.1` como segunda cinta. No reasignar Buffer/Blade Stop hasta confirmar su nuevo mapeo físico y en TIA.
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

HOST (Windows 10/11 - Tarjeta Gráfica GPU)
┌──────────────────────────────────────────────────────────────┐
│ Machine Simulator (Nirtec) v5                                │
│ - Driver: Siemens S7 (TCP/IP / Ethernet)                     │
│   IP Destino: 192.168.15.138 (IP de la VM)                   │
│   Puerto: 102 | Rack: 0 | Slot: 1                            │
│   Offsets: Start byte 10 (Inputs y Outputs)                  │
└──────────────┬──────────────────────────────▲────────────────┘
│ TCP :102                     │ Modbus TCP :502
▼                              ▼
┌──────────────────────────────┐  ┌────────────────────────────┐
│ VM VMware (Modo Bridged)     │  │ MESA DE TRABAJO (FÍSICA)   │
│ IP: 192.168.15.138           │  │ ESP32 NodeMCU (WROOM-32)   │
│                              │  │ - Botonera Start / Stop    │
│  NetToPLCsim (Admin)         │  │ - Módulo Relés 5V          │
│  - Network IP: 192.168.15.138│  │ - Sensor Inductivo (Fallo) │
│  - PLCSIM IP:  192.168.0.1   │  │ - Motor NEMA 17 + A4988    │
│  - Rack 0 / Slot 1           │  └────────────────────────────┘
│         ▲                    │
│         ▼                    │
│  S7-PLCSIM (CPU S7-1511-1 PN)│
│  IP: 192.168.0.1             │
│  TIA Portal V21              │
└──────────────────────────────┘


### 6.2. Procedimiento Paso a Paso: Conexión Machine Simulator (Host) ↔ VMware (TIA Portal + PLCSIM)
1. **Configuración de Hardware y Permisos en TIA Portal (Dentro de la VM):**
   * En el árbol del proyecto, clic derecho en el PLC `CPU 1511-1 PN` > **Properties**[cite: 7].
   * En *Protection & Security > Connection mechanisms*, marcar obligatoriamente **"Permit access with PUT/GET communication from remote partner"**[cite: 7].
   * En *Protection & Security > Access control*, seleccionar **Full access (no protection)**[cite: 7].
   * En las propiedades generales del proyecto (clic derecho sobre el nombre raíz del proyecto > Properties > Protection), verificar que esté marcada la casilla **"Support simulation during block compilation"**[cite: 7, 9].
   * Compilar hardware y software por completo: clic derecho en PLC > **Compile > Hardware and software (rebuild all)**[cite: 7].
2. **Arranque y Puesta en RUN de S7-PLCSIM (Dentro de la VM):**
   * Iniciar la simulación desde el icono *Start simulation* o clic derecho en el PLC > *Start simulation*[cite: 7, 9].
   * En la ventana *Extended download to device*:
     * *PG/PC interface:* Seleccionar `Siemens PLCSIM Virtual Ethernet Adapter` (o `PLCSIM` si no se bloquea)[cite: 7].
     * *Connection to interface/subnet:* Seleccionar `PN/IE_1` (o subred configurada)[cite: 7].
     * Pulsar **Start search**, seleccionar el autómata con IP `192.168.0.1` en la tabla y pulsar **Load**[cite: 7].
   * En la ventana final de carga, en la columna *Action* seleccionar **Start module** y pulsar **Finish**[cite: 7].
   * *Verificación:* En la ventana de S7-PLCSIM, la `Instance_1` debe mostrar el indicador LED en verde fijo (**RUN**)[cite: 7].
3. **Liberación de Puerto y Arranque de NetToPLCsim (Dentro de la VM):**
   * Asegurarse de que el Firewall de Windows en la VM esté desactivado para redes públicas y privadas[cite: 5, 7].
   * Abrir `NetToPLCsim.exe` con clic derecho > **Ejecutar como administrador**[cite: 7].
   * Si salta el aviso *"Port 102 is in use"*, aceptar para detener el servicio de Windows `SIMATIC S7DOS Help Service`[cite: 7].
   * Pulsar **Add**:
     * *Network IP Address:* Seleccionar la IP local de la VM (ej. `192.168.15.138` obtenida mediante `ipconfig` en CMD)[cite: 5, 7].
     * *Plcsim IP Address:* Escribir la IP interna del PLC simulado (`192.168.0.1`)[cite: 5, 7].
     * *Plcsim Rack/Slot:* Configurar obligatoriamente **Rack 0** y **Slot 1** (la CPU del S7-1500 reside siempre en el Slot 1; Slot 2 provocará rechazo de conexión)[cite: 5, 7].
     * *Enable TSAP check:* Desmarcado[cite: 5].
   * Pulsar **OK** y después **Start server**[cite: 5, 7].
   * *Verificación:* El estado de NetToPLCsim pasa a `RUNNING` y el puerto 102 marca `OK`[cite: 5, 7].
4. **Configuración del Driver de Siemens en Machine Simulator (En el PC Host):**
   * Abrir Machine Simulator y en la barra superior acceder a **DRIVERS E/S** (o *Drivers*)[cite: 5, 7].
   * En el desplegable de controladores, seleccionar **Siemens S7 (TCP/IP / Ethernet)** y pulsar en **Configure / Settings**[cite: 5, 7].
   * Introducir los parámetros de enlace:
     * *IP Address / Host:* La IP física de la VM (`192.168.15.138`), **nunca** la 192.168.0.1[cite: 5, 7].
     * *Port:* `102`[cite: 5, 7].
     * *Rack:* `0` | *Slot:* `1`[cite: 5, 7].
     * *Network Adapter:* Seleccionar la tarjeta de red física activa del host (Realtek, Ethernet o Wi-Fi), nunca *Loopback*[cite: 7, 11].
   * **Offsets de memoria (Contrato del Byte 10):**
     * *S7 Inputs/EasyPLC Driver Outputs (Start byte):* `10`[cite: 5, 7].
     * *S7 Outputs/EasyPLC Driver Inputs (Start byte):* `10`[cite: 5, 7].
     * *Byte length:* `2` (o `4`)[cite: 7].
   * Pulsar **OK** para cerrar ajustes y conectar el driver[cite: 7].
   * *Verificación:* El indicador de estado pasa a **Connected** (verde). Al pulsar **Play** en la escena 3D, las cintas y actuadores responderán en tiempo real a las salidas `%Q10.x` gobernadas desde TIA Portal[cite: 5, 7].

### 6.3. Procedimiento Paso a Paso: Restauración del Snapshot de la VM y Gestión de Licencia
1. **Backup Preventivo del Proyecto (Antes del Día 20 de la Trial):**
   * Abrir el proyecto en TIA Portal dentro de la máquina virtual[cite: 5].
   * Ir al menú superior **Project > Archive** y generar el archivo comprimido `.zap21`[cite: 5].
   * Mover el archivo `.zap21` a la **Carpeta Compartida** (`\\vmware-host\Shared Folders\Factory-VM`)[cite: 5].
   * *Verificación crítica:* Comprobar desde el explorador de Windows del PC físico (Host) que el archivo `.zap21` o proyecto está físicamente a salvo en el disco SSD principal[cite: 5].
2. **Apagado del Sistema Invitado:**
   * Cerrar TIA Portal y Automation License Manager[cite: 5].
   * En el Windows de la VM, ir a Inicio > **Apagar**[cite: 5]. Esperar a que la ventana de VMware quede en negro por completo[cite: 5].
3. **Restauración del Snapshot ("Día 1 - TIA Limpio"):**
   * En VMware Workstation, con la VM apagada, ir al menú superior: **VM > Snapshot > Snapshot Manager** (o *VM > Snapshot*)[cite: 5].
   * Seleccionar la instantánea original de guardado **"Día 1 - TIA Limpio"** (tomada con TIA Portal y PLCSIM instalados al 100% pero sin haber sido abiertos nunca)[cite: 5, 9].
   * Pulsar **Restore / Go to** y confirmar[cite: 5].
   * *Verificación:* VMware revierte el disco `.vmdk` al estado inicial virgen en pocos segundos[cite: 5].
4. **Reactivación de la Prueba de 21 Días y Recuperación:**
   * Encender la máquina virtual[cite: 5].
   * Reconfigurar la carpeta compartida si fuera necesario (*VM > Settings > Options > Shared Folders > Always enabled*)[cite: 5].
   * Arrastrar el proyecto `.zap21` desde la carpeta compartida al disco virtual[cite: 5].
   * Abrir TIA Portal. Al saltar la ventana de *Automation License Manager*, seleccionar **STEP 7 Professional** y pulsar **Activate Trial (21 days)**[cite: 5].
   * *Comprobación:* En Automation License Manager (ALM), en la unidad `C:`, verificar que la licencia de prueba figura activa con validez de 21 días[cite: 5].
5. **Plan de Contingencia ("Tierra Quemada" / Reinstalación Limpia):**
   * Si en futuras revisiones el ALM detecta anomalías de reloj y marca la licencia como *Defective*, descartar el Snapshot[cite: 5].
   * Eliminar la máquina virtual por completo y crear una nueva VM con Windows limpio (o restaurar un snapshot "Windows Virgen" previo a Siemens)[cite: 5].
   * Al reinstalar TIA Portal y PLCSIM desde los instaladores `.iso` guardados en el host, VMware generará un nuevo UUID y MAC virtual, garantizando 21 días oficiales al 100% de efectividad sin coste[cite: 5].

---

## 7. Mapa de Entradas y Salidas

### Entradas Digitales (PLC Inputs)
* `%I10.0` - `IN_HIL_BtnStart` (BOOL): Pulsador físico real en protoboard [Confirmado].
* `%I10.1` - `IN_HIL_BtnStop` (BOOL): Pulsador físico real de parada/rearme [Confirmado].
* `%I10.2` - `IN_HIL_SensorFault` (BOOL): Sensor inductivo físico (inyección de fallos) [Confirmado].
* `%I10.3` - `IN_Sensor_Presencia` (BOOL): Trigger cenital de clasificación de altura [Validado; asignación anterior de BufferPre pendiente de armonización].
* `%I10.4` - `IN_Sensor_Height_S` (BOOL): Fotocélula inferior de clasificación de altura [Validado].
* `%I10.5` - `IN_Sensor_Height_M` (BOOL): Fotocélula intermedia de clasificación de altura [Validado].
* `%I10.6` - `IN_Sensor_Height_L` (BOOL): Fotocélula superior de clasificación de altura [Validado].
* `%I10.7` - `IN_Sensor_FillPos` (BOOL): Detección de botella centrada en boquilla [Propuesta técnica].
* `%I11.0` - `IN_Sensor_ScaleStop` (BOOL): Fotocélula de centrado en báscula [Confirmado][cite: 7].
* `%I11.1` - `IN_Cobot_Done` (BOOL): Señal de fin de ciclo de taponado [Confirmado].
* `%I11.2` - `IN_Sensor_ExitDetect` (BOOL): Fotocélula de evacuación final [Confirmado].

### Salidas Digitales (PLC Outputs)
* `%Q10.0` - `Cinta1` / `OUT_Conveyor_Entry` (BOOL): Motor de transporte continuo [Validado en OB1].
* `%Q10.1` - `Cinta2_DeteccionSize` (BOOL): Motor de la cinta de detección de altura [Validado en OB1; mapeo de BladeStop pendiente de armonización].
* `%Q10.2` - `OUT_Nozzle_Down` (BOOL): Descenso vertical boquilla de dosificación [Confirmado].
* `%Q10.3` - `OUT_Valve_Liquid` (BOOL): Electroválvula de llenado de líquido [Confirmado].
* `%Q10.4` - `OUT_Conveyor_Scale` (BOOL): Motor de cinta sobre báscula [Confirmado][cite: 7].
* `%Q10.5` - `OUT_Pusher_Reject` (BOOL): Empujador neumático descarte a 90° [Confirmado].
* `%Q10.6` - `OUT_Cobot_Start` (BOOL): Disparo de ciclo de cierre/roscado del cobot [Confirmado].
* `%Q10.7` - `OUT_HIL_RelayAlarm` (BOOL): Relé 1 / Maniobra física de alarma o rechazo [Confirmado].
* `%Q11.0` - `OUT_HIL_RelayCycle` (BOOL): Relé 2 / Confirmación física de ciclo en marcha [Confirmado].

### Registros Analógicos y Posicionamiento
* `%IW64` - `ANA_Tank_Level` (INT, 0-27648): Nivel transmisor depósito (0.0 - 100.0 %) [Confirmado].
* `%QW64` - `CMD_Pump_Inflow` (INT, 0-27648): Salida analógica PID bomba/válvula [Confirmado].
* `%ID100` o `%IW66` - `ANA_Scale_Weight` (REAL/INT): Medida de masa en báscula (0.0 - 2000.0 g) [Confirmado][cite: 7].
* `%QW66` - `HIL_NEMA17_Steps` (INT): Consigna angular para motor paso a paso espejo [Validado en Modbus].

---

## 8. Secuencias de Funcionamiento y Lógica PLC
* **Estructura en TIA Portal:**
  * `OB1`: Bucle determinista maestro, modos de marcha, llamadas a secuenciadores y gestión de interlocks[cite: 7].
  * `OB30` (Interrupción cíclica a 100 ms): Ejecución dedicada del lazo `PID_Compact` de nivel de líquido.
  * `FB_Buffer_Manager` (Ladder / KOP): Máquina de estados (GRAFCET) para retención y suelta unitaria de piezas.
  * `FB_Quality_Engine` / `FC_Analog_Scaling` (SCL): Tratamiento analógico con `NORM_X`/`SCALE_X`, evaluación matricial de tolerancias y asignación de receta.
  * `DB_Tracking_FIFO`: Registro dinámico para correlacionar el envase medido con la botella que entra a la báscula.

---

## 9. Hardware Físico e Integración HIL
* **Microcontrolador ESP32 (WROOM-32):** Nodo esclavo Modbus TCP sobre Wi-Fi local (puerto 502) con temporizador *Watchdog/Keep-Alive* (1500 ms)[cite: 5].
* **Módulo de Relés 5V (4 canales optoacoplados):** Maniobra de conmutación audible ante descartes y señalización física, alimentado en riel externo independiente para prevenir *brownouts*.
* **Instrumentación de campo:** Pulsadores cableados en pull-up interno hacia masa; sensor inductivo industrial tubular con divisor resistivo de tensión para adaptar señal de 12/24V a 3.3V GPIO[cite: 5].
* **Cinemática espejo:** Motor bipolar NEMA 17 gobernado por driver A4988 (pines Step/Dir en GPIOs 25, 26 y 27) girando en sincronía con el proceso virtual.
* **Infraestructura de taller auxiliar:** Osciloscopio/multímetro FNIRSI 2C53T (3 en 1 portátil), fuente lineal regulable 30V/5A, estación de soldadura rápida T12.

---

## 10. Decisiones Vigentes y Motivos
1. **Selección de Machine Simulator Suite:** Solución de licencia fija (92 €), importación CAD de envases reales y drivers nativos sin sobrecarga gráfica[cite: 5].
2. **Contrato del Byte 10:** Blindaje de memoria frente a la sobreescritura interna de Siemens PLCSIM[cite: 7, 11].
3. **Proceso híbrido continuo/discreto:** Inclusión del lazo PID de nivel para maximizar el rigor técnico y la densidad lógica del proyecto.
4. **Repositorio centralizado en GitHub (`gemelo-digital-lab`):** Trazabilidad integral de código C++, exportaciones SCL, documentación y esquemas eléctricos.

---

## 11. Propuestas Descartadas o Sustituidas y Motivos
* **Maqueta extensa de 5 ramales en Factory I/O:** Descartada por saturación física (caída de FPS < 25) e inviabilidad de coste por suscripción recurrente[cite: 5].
* **Uso no preventivo de Snapshots:** Queda prohibido tomar instantáneas con la VM encendida o intentar alterar el reloj interno de Windows. Los snapshots se ejecutan estrictamente en frío sobre el "Día 1" virgen tras exportar los proyectos fuera del entorno virtual[cite: 5].

---

## 12. Problemas Encontrados y Soluciones Probadas
1. **Bloqueo del puerto TCP 102 en la VM:** Conflicto con el servicio `SIMATIC S7DOS Help Service`. Solución: detención previa del servicio y ejecución de NetToPLCsim como Administrador[cite: 7].
2. **Rechazo de conexión en NetToPLCsim por Slot incorrecto:** Denegado al configurar Slot 2. Solución: fijar Slot 1 correspondiente a la CPU S7-1500[cite: 7, 11].
3. **Pérdida de comunicación VM-Host:** Fallos en adaptador NAT. Solución: configurar adaptador de VMware en modo Bridged sobre la interfaz de red física activa[cite: 7, 11].
4. **Valores analógicos y digitales forzados a cero:** Solucionado mediante el desplazamiento de offset al Byte 10[cite: 7, 11].
5. **Bloqueo al descargar a PLCSIM:** TIA Portal no detectaba el autómata. Solución: seleccionar `Siemens PLCSIM Virtual Ethernet Adapter` en PG/PC interface y detener temporalmente NetToPLCsim durante la descarga del programa[cite: 7].

---

## 13. Dudas y Tareas Pendientes
* **Resolución del dilema cinemático de packaging:** Selección final entre pórtico cartesiano XYZ (matrices en SCL) y brazo robótico de 6 ejes con garra de vacío.
* **Estación de altura:** implementar tres contadores ascendentes `CTU` (S/M/L) en `DB_Production` para productividad acumulada.
* **Estación 4 — Báscula:** acoplar la báscula dinámica e implementar parada de 1.0 s con TON para estabilizar la célula de carga antes de evaluar masa y tolerancias.
* **Mapa E/S:** resolver la reasignación de `IN_Sensor_BufferPre` y `OUT_BladeStop` tras incorporar `IN_Sensor_Presencia` y la segunda cinta.
* **Pruebas institucionales en UPC EEBE:** Coordinación con el departamento para transferir el proyecto a un autómata SIMATIC S7-1500 real.

---

## 14. Riesgos Técnicos y Pruebas Necesarias
* **Determinismo de red:** Comprobación de que la latencia acumulada entre Machine Simulator, NetToPLCsim y el ESP32 no exceda los 50 ms.
* **Saturación de recursos de cómputo:** Monitoreo de memoria RAM y núcleos de CPU durante la ejecución simultánea de VMware y Machine Simulator a 60 Hz[cite: 9].

---

## 15. Cronología de Avances
* *Septiembre 2026:* Definición de arquitectura base IT/OT y adquisición escalonada de componentes de instrumentación[cite: 5, 8].
* *Octubre 2026 (Semana 1):* Configuración de la máquina virtual VMware, pasarela NetToPLCsim en puerto 102 y primer lazo cerrado HIL operativo[cite: 7, 11].
* *Octubre 2026 (Semana 2):* Depuración de la estación Buffer en Ladder y decisión de migración a Machine Simulator[cite: 5].
* *Octubre 2026 (Semana 3):* Rediseño hacia la célula flexible de embotellado con lazo PID y redacción de propuesta para TFG en la UPC EEBE.
* *Octubre 2026 (Semana 4):* Escena 3D ensamblada en Machine Simulator, verificación del enlace S7 Ethernet hacia PLCSIM V21 y preparación de bloques en SCL[cite: 7].
* *Noviembre 2026 (3):* Clasificación de altura S/M/L validada al vuelo con trigger cenital y `FC_HeightClassification`; queda pendiente armonizar el mapa global de Buffer/Blade Stop.

---

## 16. Fuentes Revisadas
1. Historial del cuaderno de trabajo y sesiones de diseño metodológico IT/OT[cite: 5, 7, 11].
2. Documento de propuesta técnica de célula de embotellado (`Propuesta Gemelo Digital Embotellado (Machine Simulator + TIA Portal)_MiguelDanielAlvarez.gdoc` y `.pdf`).
3. Formulación técnica académica inicial para la UPC EEBE (`Propuesta Técnica - Gemelo Digital HIL IT-OT (UPC EEBE).gdoc`).
4. Repositorio GitHub `migxelad/gemelo-digital-lab` (`AGENTS.md` y `HITOS.md`).
