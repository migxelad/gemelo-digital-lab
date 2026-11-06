# Backup de TIA Portal

La carpeta está destinada a backups comprimidos de TIA Portal V21 con extensión `.zap21`.

El archivo versionado anteriormente como `SortreingByWeight.ap21` se ha eliminado porque es un proyecto/base de datos suelto, no un backup comprimido oficial. No se renombra a `.zap21`: cambiar la extensión no convierte el formato. Para completar este respaldo, exporta desde TIA Portal el proyecto `GemeloDigitalEnsamblado.maq` correspondiente a la célula vigente y guarda el archivo comprimido aquí, con un nombre descriptivo, por ejemplo `GemeloDigitalEnsamblado.zap21`. El archivo fuente de Machine Simulator está en `machine_simulator/`.
