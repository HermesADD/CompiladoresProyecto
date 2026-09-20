# Práctica 2 — Analizador léxico completo de MiniC

## Información general

| Campo | Información |
|---|---|
| Asignatura | Compiladores |
| Número de práctica | 2 |
| Semestre | 2027-1 |

## Integrantes

| Nombre completo | Número de cuenta | Correo electrónico |
|---|---|---|
|Hermes Alberto Delgado Díaz | 319258613 | hermesdelgado@ciencias.unam.mx |
|Joaquín Medina Peralta | 320202513 | joaquin.medina513@ciencias.unam.mx |


## Estructura del proyecto

```text
Practica01/
├── README.md
├── CHANGELOG.md
├── reporte.pdf
├── Makefile
├── include/
│   └── lexer/
│       ├── lexer.h
│       └── token.h
├── src/
│   ├── main.c
│   └── lexer/
│       ├── lexer.c
│       └── token.c
└── tests/
    ├── README.md
    ├── run_tests.sh
    ├── public/
    │   ├── inputs/
    │   └── expected/
    └── team/
        ├── inputs/
        └── expected/
```

### Módulos implementados

| Archivo o módulo | Responsabilidad |
|---|---|
| `src/main.c` | Valida argumentos, abre el archivo en modo binario, configura `stdout` en Windows, invoca al lexer, cierra el archivo y devuelve el código de salida. |
| `include/lexer/token.h` | Define `TokenType`, la estructura `Token` y la interfaz de manejo de tokens. |
| `src/lexer/token.c` | Implementa creación, impresión, liberación de tokens y conversión de `TokenType` a nombre textual. |
| `include/lexer/lexer.h` | Declara la interfaz pública del analizador léxico. |
| `src/lexer/lexer.c` | Recorre el archivo, mantiene línea/columna, ignora espacios y comentarios, reconoce enteros, identificadores, palabras reservadas, símbolos compuestos y símbolos simples, genera `ERROR` y `TOKEN_EOF`. |
| `tests/public/run_public_tests.sh` | Ejecuta los casos públicos y compara de forma exacta la salida producida contra los archivos esperados. |

## Requisitos

### Linux / WSL

- GCC con soporte para C11.
- GNU Make.
- Bash y `diff` para ejecutar las pruebas.

En distribuciones basadas en Debian/Ubuntu, normalmente basta con contar con `build-essential`.

### Windows

La forma recomendada de ejecutar el proyecto es mediante uno de estos entornos:

- **WSL**, usando GCC, Make y Bash como en Linux; o
- **MSYS2/MinGW64**, con GCC, Make, Bash y `diff` disponibles.

El script `tests/run_tests.sh` usa Bash, por lo que no está diseñado para ejecutarse directamente desde `cmd.exe` o PowerShell sin un entorno Unix compatible.

Dependencias externas para el lexer: **ninguna**.

## Compilación

Para compilar el proyecto:
```bash
make
```

Después de la compilación se genera el ejecutable minic.

Para eliminar los archivos generados durante la compilación:

```bash
make clean
```

Ejecución

El programa recibe como argumento la ruta de un archivo fuente de MiniC.

Ejemplo:

```bash
./minic programa.mc
```

Cada token distinto de TOKEN_EOF se imprime en stdout con el siguiente formato:

línea:columna TIPO_TOKEN lexema

Por ejemplo:

```text
1:0 INTEGER 10
1:3 PLUS +
1:5 INTEGER 20
1:7 SEMICOLON ;
```
El token TOKEN_EOF no tiene lexema y se imprime con el formato:

```text
línea:columna TOKEN_EOF
```

Por ejemplo:

```text
1:8 TOKEN_EOF
```

Los errores relacionados con argumentos incorrectos o con la apertura del archivo se escriben en stderr, de modo que no se mezclen con la secuencia de tokens generada por el analizador.

## Funcionalidades implementadas

- [X] Las pruebas de la Práctica 1 continúan aprobándose.
- [X] Los identificadores utilizan máxima coincidencia.
- [X] Las palabras reservadas se clasifican después de reconocer el lexema.
- [X] Los operadores compuestos se reconocen antes que los simples.
- [X] `!`, `&` y `|` aislados producen `ERROR`.
- [X] `/` y `//` se distinguen correctamente.
- [X] Los comentarios no producen tokens.
- [X] Los errores no detienen el análisis.
- [X] Línea y columna se conservan correctamente.
- [X] Se produce exactamente un `TOKEN_EOF`.
- [X] La salida estándar coincide exactamente con el formato publicado.
- [X] El proyecto compila con las opciones mínimas exigidas.

## Pruebas

El proyecto contiene **10 pruebas públicas** y **5 pruebas propias**. Para ejecutar todas:

```bash
make test
```

También puede ejecutarse directamente:

```bash
bash tests/run_public_tests.sh
```

El runner redirige la salida real a un archivo temporal y la compara con `diff -u`, de modo que la validación conserva y detecta diferencias de bytes como saltos de línea extra o caracteres `\r` inesperados.

En la revisión de esta entrega se obtuvo:

```text
Aprobadas: 13
Fallidas: 0
```

Consulte `tests/public/README.md` para el detalle de cada caso.

## Portabilidad y finales de línea (Windows/Linux)

La práctica exige que `\r\n` se trate como **un solo salto de línea**, que un `\r` aislado también cuente como salto y que la salida coincida exactamente con los archivos `.out`. Esto genera varios puntos de atención al trabajar entre Windows y Linux.

### 1. Entrada CRLF frente a LF

Windows suele utilizar CRLF (`\r\n`) y Linux suele utilizar LF (`\n`). El archivo fuente se abre con:

```c
fopen(argv[1], "rb")
```

El modo binario evita que la biblioteca de C traduzca los finales de línea antes de que el lexer los procese. Así, el analizador recibe los bytes reales y puede aplicar explícitamente las reglas de la práctica para `\r`, `\n` y `\r\n`.

### 2. Salida de `stdout` en Windows

En Windows, un flujo de texto puede traducir un `\n` escrito por `printf` a `\r\n`. Aunque visualmente la salida parezca igual, una comparación exacta contra archivos `.out` con LF puede fallar.

Por esa razón `src/main.c` usa, únicamente en Windows:

```c
_setmode(_fileno(stdout), _O_BINARY);
```

Con esto, la salida generada conserva exactamente los `\n` que imprime el programa. En Linux esta configuración no se compila y no modifica el comportamiento.

### 3. Conversión automática de Git (`core.autocrlf`)

Git puede convertir finales de línea durante `checkout` o `commit`. Esto es especialmente delicado en esta práctica porque algunos archivos `.mc` de prueba contienen CRLF **intencionalmente**.

Se incluye `.gitattributes` para:

- mantener `*.c`, `*.h`, `*.md`, `Makefile`, `*.sh` y `*.out` con LF;
- impedir que Git normalice los archivos `tests/*/inputs/*.mc`, ya que sus bytes forman parte de lo que se está probando;
- evitar que `run_tests.sh` termine con CRLF, situación que en Linux puede provocar errores como `bash\r` o interpretaciones incorrectas del script.

### 4. Herramientas de prueba en Windows

El Makefile y `tests/run_tests.sh` utilizan utilidades de entorno Unix (`bash`, `diff`, `rm`, `mkdir`). Por ello, para Windows se recomienda **WSL** o **MSYS2/MinGW64**. El lexer en C contiene la adaptación necesaria para la salida de Windows, pero el entorno de automatización de pruebas no pretende ser nativo de PowerShell/CMD.

## Problemas conocidos / limitaciones

- El lexer reconoce únicamente el subconjunto solicitado para la Práctica 1. Identificadores, palabras reservadas, operadores compuestos y comentarios se implementarán en la Práctica 2
- Las pruebas automatizadas requieren Bash y `diff`; en Windows se debe usar WSL o un entorno tipo MSYS2/Git Bash que proporcione esas herramientas.
- La configuración de finales de línea depende de conservar el archivo `.gitattributes` al clonar o copiar el repositorio. Si los fixtures `.mc` son modificados por un editor que normaliza finales de línea, las pruebas específicas de CRLF pueden dejar de representar el caso original.

## Notas de ejecución

- Verificado con `gcc -std=c11 -Wall -Wextra -Wpedantic` sin advertencias.
- Verificado en Linux con 13/13 pruebas exitosas.
- `make clean` elimina `build/` y el ejecutable generado.
- No se entregan binarios, archivos objeto ni contenido de `build/`.