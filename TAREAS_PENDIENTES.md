# Tareas pendientes y punto de continuación

**Proyecto:** `gemelo-digital-lab`  
**Entorno:** TIA Portal V21 / S7-PLCSIM en VMware, NetToPLCsim TCP 102 y Machine Simulator Suite v5.  
**Estado contrastado con el repositorio:** 2026-11-03.  
**Fuente de tags:** captura vigente de PLC tags descrita en [CONTEXTO_MAESTRO.md](CONTEXTO_MAESTRO.md). Los mapas anteriores y el XML exportado por Factory I/O están obsoletos.

## Estado ya alcanzado

- [x] Retirar de seguimiento la escena antigua de Factory I/O y su XML de tags obsoleto.
- [x] Ordenar el contexto como `CONTEXTO_MAESTRO.md`, organizar los recursos visuales y añadir `.gitignore`.
- [x] Actualizar README y registrar la clasificación de altura al vuelo en `HITOS.md`.
- [x] Validar en la Watch Table que una caja L produce `FormatID = 3` sin detener la cinta.
- [x] Versionar `FC_HeightClassification.scl`: trigger `IN_Sensor_Presencia`, prioridad L→M→S, receta en `DB_Production.CurrentRecipe` y retención cuando no hay presencia.

**Pendiente de esta sección de repositorio:** subir la escena nativa actual de Machine Simulator y un backup comprimido de TIA Portal. Las carpetas están preparadas, pero esos archivos no se han proporcionado. Los dos MP4 actualmente en `docs/videos/` son placeholders de 2 bytes; reemplazarlos por las grabaciones originales.

## Próxima sesión — cerrar contadores de clasificación

### 1. Verificar el estado real del proyecto TIA

- [ ] Abrir `DB_Production`, `Main [OB1]`, `FC_HeightClassification [FC1]` y `typeRecipe` en la versión actual de TIA Portal.
- [ ] Confirmar si `Count_Small`, `Count_Medium`, `Count_Large` y `Total_Inspected` existen ya en el DB. Si faltan, declararlos como `DINT` y exportar el cambio SCL/UDT o documentación compatible.
- [ ] Confirmar el uso previsto de `Lectura_Completada` (`%M10.1`) en el proyecto. No reasignar esta marca ni asumir que ya evita dobles conteos sin revisar OB1/FC1.

### 2. Implementar el conteo por formato

- [ ] Generar un único evento de conteo por caja, con detección de flanco positivo de `IN_Sensor_Presencia` (`%I10.3`) o una estrategia equivalente validada en la secuencia actual.
- [ ] Capturar el `FormatID` correspondiente a esa lectura y activar el contador ascendente del formato:
  - `FormatID = 1` → `CTU_Small` → `Count_Small`.
  - `FormatID = 2` → `CTU_Medium` → `Count_Medium`.
  - `FormatID = 3` → `CTU_Large` → `Count_Large`.
- [ ] Incrementar `Total_Inspected` una sola vez por caja clasificada. Definir el comportamiento de reset de contadores antes de implementarlo.
- [ ] Mantener la llamada de clasificación y el mapa de E/S vigentes en [CONTEXTO_MAESTRO.md](CONTEXTO_MAESTRO.md); no reintroducir tags del mapa antiguo.

### 3. Validar el conteo en la simulación

- [ ] Observar S, M y L en la Watch Table y comprobar un incremento por caja.
- [ ] Dejar la presencia activa durante varios ciclos PLC para comprobar que no se cuentan duplicados.
- [ ] Comprobar que el siguiente envase puede clasificarse y contarse sin limpiar la receta ni detener cintas.

## Después — dosificación y báscula

### Dosificación y actuadores

- [ ] Inspeccionar en Machine Simulator los actuadores reales del cabezal y el retenedor.
- [ ] Añadir a la tabla PLC solo los tags y direcciones de actuadores que se confirmen en la escena y en TIA; respetar E/S de proceso a partir de `%Q10.0`.
- [ ] Vincular la secuencia de llenado a `DB_Production.CurrentRecipe.FillTime` y validar el ciclo.

### Báscula dinámica y descarte

- [ ] Confirmar en TIA y Machine Simulator el tag de peso, tipo de dato, escala/unidad, tara y peso nominal objetivo por formato.
- [ ] Confirmar direcciones actuales para presencia/centrado, cinta de báscula, captura de peso y empujador. Las propuestas `%ID100`, `%I11.0`, `%Q10.2`, `%Q10.4` y `%Q10.5` del borrador recibido **no están validadas por la captura actual**; tratarlas como hipótesis hasta verificarlas.
- [ ] Diseñar el secuenciador Ladder de entrada, parada y estabilización de 1.0 s con TON, evaluación y salida PASS/REJECT.
- [ ] Implementar `FC_WeightEvaluation` en SCL comparando peso medido con el objetivo confirmado y `CurrentRecipe.Tolerance_g`.
- [ ] Validar PASS, REJECT, estabilización, reanudación de cinta y expulsión por el `WorkPart Destroyer`.

## Decisiones todavía abiertas

- [ ] Resolver la numeración de estaciones: el contexto maestro llama a pesaje “Estación 4”, mientras que la lista original de Gemini lo enumera como “Estación 3”. Usar por ahora el nombre funcional “báscula dinámica”.
- [ ] Elegir la cinemática de packaging final.
- [ ] Planificar la transferencia y pruebas en un S7-1500 físico.

## Reglas vigentes

- El mapa de tags más reciente es el que aparece en [CONTEXTO_MAESTRO.md](CONTEXTO_MAESTRO.md); no usar direcciones anteriores no presentes en esa tabla.
- Las E/S digitales de proceso deben comenzar en Byte 10.
- Los cambios de arquitectura y memoria también deben quedar reflejados en `CONTEXTO_MAESTRO.md` y en `HITOS.md`.
