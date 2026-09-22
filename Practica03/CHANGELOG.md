# Changelog

Este proyecto sigue un progreso por práctica.

## [3.0.0] - Práctica 3 (Analizador Sintáctico)

### Añadido
- Implementación de un analizador sintáctico descendente recursivo para el lenguaje MiniC.
- Reconocimiento de gramática de sentencias: declaraciones, asignaciones, instrucción `print`, condicionales (`if`/`else`), ciclos (`while`) y bloques de código.
- Reconocimiento de gramática de expresiones evaluadas por niveles de precedencia y asociatividad.
- Reporte detallado de errores sintácticos indicando línea, columna, elemento esperado y token inválido encontrado.
- Sistema de recuperación ante errores y sincronización mediante tokens clave para evitar estancamientos y cascadas de mensajes de error.
- Integración de revisión de memoria en el `Makefile` mediante el comando `make memcheck` asistido por Valgrind.

### Modificado
- Refactorización del diseño del `Lexer` para actuar como un productor incremental de tokens en lugar de procesar el archivo monolíticamente.
- Exposición de la estructura completa de `Lexer` en `lexer.h` para resolver problemas de tamaño de almacenamiento en `main.c`.
- Reestructuración de `main.c` para gestionar el ciclo de vida, la inicialización y la destrucción encadenada del analizador léxico y el sintáctico.
- Actualización del script `run_public_tests.sh` para capturar de manera combinada `stdout` y `stderr`, permitiendo regresiones sobre errores sintácticos.

## [2.0.0] - Práctica 2 (Analizador Léxico)

### Añadido
- Se agrego en el lexer la lectura de identificadores, palabras reservadas, operadores compuestos y además de agregar el caso de uso de comentarios de una sola línea y el manejo de la posición del lexema.

### Corregido
- Se modifico la función `simple_token_type` la cual elimina los casos de las operaciones que comparten caracteres con la operaciones compuestas, lo cual se maneja en la función `evaluate_compound_operator`

### Documentación
- README ampliado con descripción del proyecto, módulos, compilación, ejecución, funcionalidades y pruebas.
- Instrucciones separadas para Linux/WSL y Windows con MSYS2/MinGW64.
- Sección específica sobre problemas de portabilidad entre CRLF (`\r\n`) y LF (`\n`).
- Explicación del efecto de `core.autocrlf`/normalización de Git sobre los archivos de pruebas.
- Documentación de la necesidad de conservar `run_public_tests.sh` con LF para evitar problemas de ejecución en Linux.
- Limitaciones de las pruebas automatizadas desde PowerShell/CMD documentadas; se recomienda WSL o un entorno Bash compatible.

## [1.0.0] - Práctica 1

### Añadido
- Estructura modular del proyecto (`include/`, `src/`, `tests/`).
- Enumeración completa `TokenType` con todas las categorías léxicas
  requeridas por la especificación de MiniC (aunque en esta entrega solo
  se reconoce un subconjunto).
- Estructura `Token` y funciones asociadas: `token_init`, `token_print`,
  `token_destroy`, `token_type_name`.
- Lectura del archivo fuente desde la línea de comandos, con validación de
  argumentos y de apertura del archivo (`main.c`).
- Recorrido del archivo carácter por carácter con seguimiento de línea y
  columna (`lexer.c`), incluyendo el tratamiento de `\r\n` como un único
  salto de línea y de `\r` aislado como salto de línea.
- Manejo de espacios en blanco (espacio, tabulador, `\n`, `\r`) sin
  generación de tokens.
- Reconocimiento de símbolos simples de un carácter: `PLUS`, `MINUS`,
  `STAR`, `SLASH`, `ASSIGN`, `LESS`, `GREATER`, `LPAREN`, `RPAREN`,
  `LBRACE`, `RBRACE`, `SEMICOLON`.
- Reconocimiento de números enteros sin signo (`[0-9]+`), consumiendo
  todos los dígitos consecutivos como un único token `INTEGER`.
- Generación de un token `ERROR` por cada carácter no reconocido,
  conservando el carácter, línea y columna, y recuperación del análisis
  desde el carácter siguiente.
- Generación de `TOKEN_EOF` al finalizar el archivo, con la posición
  correcta tras ignorar cualquier espacio en blanco final.
- Formato de salida exacto `línea:columna TIPO lexema` (sin lexema para
  `TOKEN_EOF`) en `stdout`; diagnósticos de uso en `stderr`.
- `Makefile` con reglas `all` (por defecto), `test` y `clean`.
- Pruebas propias del equipo en `tests/team/` (dígito único, varios
  caracteres no reconocidos consecutivos, tabuladores y espacios
  combinados con tokens de más de un carácter, y delimitadores anidados),
  además de la integración de las pruebas públicas proporcionadas por el
  curso.
- Script `tests/run_tests.sh` para ejecutar y verificar automáticamente
  todos los casos de prueba.

### Corregido

- Sustitución del diagnóstico provisional `PENDIENTE` (presente en la base
  inicial) por la generación real de tokens `ERROR`.

### Corrección portabilidad Windows/Linux

- **Salida con `\r\n` en Windows.** En Windows, `stdout` se abre por
  defecto en "modo texto", lo que traduce automáticamente cada `\n`
  escrito a `\r\n`. Esto provocaba que la salida del programa no
  coincidiera con los archivos `.out` esperados (que solo contienen
  `\n`), aun cuando el contenido era idéntico a simple vista. Se
  agregó en `src/main.c` una llamada a `_setmode(_fileno(stdout),
  _O_BINARY)`, activada únicamente al compilar en Windows mediante
  `#ifdef _WIN32`, para forzar una salida binaria idéntica en
  Windows, Linux y macOS. No afecta la compilación ni el
  comportamiento en sistemas distintos de Windows.
- Se verificó que el proyecto compila y pasa el conjunto de
  pruebas (`make test`) tanto en Linux/WSL como en Windows
  (MSYS2/MinGW64), documentando ambos flujos de compilación en el
  README.

  ### Documentación

- README ampliado con descripción del proyecto, módulos, compilación, ejecución, funcionalidades y pruebas.
- Instrucciones separadas para Linux/WSL y Windows con MSYS2/MinGW64.
- Sección específica sobre problemas de portabilidad entre CRLF (`\r\n`) y LF (`\n`).
- Explicación del efecto de `core.autocrlf`/normalización de Git sobre los archivos de pruebas.
- Documentación de la necesidad de conservar `run_tests.sh` con LF para evitar problemas de ejecución en Linux.
- Limitaciones de las pruebas automatizadas desde PowerShell/CMD documentadas; se recomienda WSL o un entorno Bash compatible.