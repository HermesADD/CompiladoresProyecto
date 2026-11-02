# CHANGELOG — Práctica 6

## [6.1.0] — Separación de pruebas públicas y del equipo

### Añadido

- Carpeta `tests/team/` con **20 pruebas propias**: 8 programas válidos y 12 inválidos.
- Archivos de resultados exactos para cada caso en `tests/team/esperados/`.
- Ejecutor `tests/team/ejecutar_pruebas.sh` que compara el estado, `stdout` y los diagnósticos completos de `stderr`.
- Catálogo de pruebas propias `tests/team/DESCRIPCION.md`.
- Caso de regresión `t20_recuperacion_sintactica` para verificar dos errores sintácticos independientes tras recuperación.
- Objetivos `make test-public`, `make test-team` y `make test-all`.

### Modificado

- Las **24 pruebas del profesor** se trasladaron a `tests/public/`, sin cambios en sus archivos ni en su ejecutor.
- `make test` ahora ejecuta ambos conjuntos (44 casos en total).
- Se actualizaron `README.md`, `tests/README.md` y las rutas usadas para ejecutar las pruebas.

### Verificación

- La batería `public` conserva el contrato original de aceptación/rechazo.
- La batería `team` comprueba resultados y diagnósticos exactos para las 20 entradas definidas.
- Las pruebas propias de recuperación sintáctica requieren que el parser continúe después de un inicializador incompleto.

## [6.0.0] — Verificación estática de tipos

### Añadido

- Los módulos `type.[ch]` y `type_checker.[ch]` sobre la base semántica de la Práctica 5.
- Verificación de `int` y `bool` en inicializaciones, asignaciones, operadores y condiciones.
- Diagnósticos con línea y columna para incompatibilidades de tipos.
- Propagación de `TYPE_ERROR` para evitar errores en cascada.
- Reutilización del análisis de símbolos, ámbitos y *shadowing* de la Práctica 5.

### Conservado

- Lexer, parser descendente recursivo, construcción del AST y administración de memoria de prácticas anteriores.
- Interfaz `./minic archivo.mc` y opción de depuración `--ast`.
