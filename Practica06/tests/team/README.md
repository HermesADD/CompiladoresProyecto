# Pruebas propias del equipo (20 casos)

Se incluyen 8 programas MiniC válidos y 12 inválidos para cubrir el analizador de tipos, declaraciones, ámbitos, precedencia de operadores y recuperación ante errores.

- `casos/`: entradas `.mc` escritas por el equipo.
- `cases.txt`: catálogo `identificador valid|invalid`.
- `esperados/*.out`: salida estándar exacta de los programas válidos.
- `esperados/*.err`: diagnósticos exactos de los programas inválidos, incluidas línea, columna y cantidad de errores.
- `DESCRIPCION.md`: finalidad de cada caso.
- `ejecutar_pruebas.sh`: ejecutor independiente.

Ejecutar desde la carpeta `Practica06`:

```sh
make test-team
```

**Criterios:** un programa válido debe salir con código 0, escribir exclusivamente `Programa semanticamente correcto.\n` por stdout y mantener stderr vacío. Un inválido debe salir con código distinto de cero, mantener stdout vacío y producir exactamente los diagnósticos documentados en su `.err`.

El caso `t08_autorreferencia_permitida` se basa en la regla del curso que considera la variable visible desde que se inserta su declaración, sin analizar si fue inicializada. El caso `t20_recuperacion_sintactica` requiere la recuperación del parser ante dos errores independientes.

Estas pruebas son adicionales a las 24 pruebas públicas del profesor y no las modifican.
