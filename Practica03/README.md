# Práctica 2 — Analizador léxico completo de MiniC

## Información general

| Campo | Información |
| --- | --- |
| Asignatura | Compiladores |
| Número de práctica | 3 |
| Semestre | 2027-1 |

## Integrantes

| Nombre completo | Número de cuenta | Correo electrónico |
| --- | --- | --- |
| Hermes Alberto Delgado Díaz | 319258613 | <hermesdelgado@ciencias.unam.mx> |
| Joaquín Medina Peralta | 320202513 | <joaquin.medina513@ciencias.unam.mx> |

## Estructura del proyecto

```text
Practica01/
├── CHANGELOG.md
├── include/
│   ├── lexer/
│   │   ├── lexer.h
│   │   └── token.h
│   └── parser/
│       └── parser.h
├── Makefile
├── README.md
├── src/
│   ├── lexer/
│   │   ├── lexer.c
│   │   └── token.c
│   ├── main.c
│   └── parser/
│       └── parser.c
└── tests/
    ├── lexer/
    │   └── public/
    │       ├── expected/
    │       ├── inputs/
    │       ├── README.md
    │       └── run_public_tests.sh
    └── public/
        ├── expected/
        ├── inputs/
        ├── README.md
        └── run_public_tests.sh

```

### Módulos implementados

| Archivo o módulo | Responsabilidad |
| --- | --- |
| `src/main.c` | Valida argumentos, abre el archivo en modo binario, configura `stdout` en Windows, invoca al lexer, invoca al parser, cierra el archivo y devuelve el código de salida junto con mensjae. |
| `include/lexer/token.h` | Define `TokenType`, la estructura `Token` y la interfaz de manejo de tokens. |
| `src/lexer/token.c` | Implementa creación, impresión, liberación de tokens y conversión de `TokenType` a nombre textual. |
| `include/lexer/lexer.h` | Declara la interfaz pública del analizador léxico. |
| `src/lexer/lexer.c` | Recorre el archivo, mantiene línea/columna, ignora espacios y comentarios, reconoce enteros, identificadores, palabras reservadas, símbolos compuestos y símbolos simples, genera `ERROR` y `TOKEN_EOF`. Además de dar la estructura para poder obtener tokens del archivo para el parser. |
| `include/parser/parser.h` | Declara la interfaz pública del analizador sintáctico, sin implementar el AST ni semántica. |
| `src/parser/parser.c` | Recorre el archivo, revisa principalmente la grámatica para MiniC, manejando los errores y la salida del programa.. |
| `tests/public/run_public_tests.sh` | Ejecuta los casos públicos y compara de forma exacta la salida producida contra los archivos esperados. |
| `tests/lexer/public/run_public_tests.sh` | Ejecuta los casos públicos y compara de forma exacta la salida producida contra los archivos esperados de las pruebas del analizador lexico. |

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

Se realiza el análisis lexico y sintáctico del programa, verificando que sea acorde a las reglas definidas para MiniC, imprimiendo en consola "Programa sintacticamente correcto.". En otro caso, indica si el error fue sintáctico o lexico en la consola de la forma:

- "Error lexico [line:column]: elemento invalido 'símbolo'
- "Error sintactico [line:column]: se esperaba 'mensaje', pero se encontro 'simbolo'

## Funcionalidades implementadas

- [X] El lexer entrega tokens incrementalmente y sin imprimirlos.
- [X] El parser no inspecciona directamente caracteres del archivo.
- [X] Cada token y lexema tiene un propietario documentado.
- [X] Existe al menos un token de anticipación.
- [X] Las funciones del parser corresponden a responsabilidades separadas.
- [X] Se reconocen todas las sentencias publicadas.
- [X] Las expresiones respetan precedencia y asociatividad.
- [X] Un token `ERROR` no genera además un diagnóstico sintáctico por sí mismo.
- [X] La recuperación no entra en ciclos ni repite indefinidamente un error.
- [X] Se consume la entrada completa hasta `TOKEN_EOF`.
- [X] Sólo los programas válidos imprimen el mensaje de éxito.
- [X] Los diagnósticos se escriben en `stderr`.
- [X] Los códigos de salida distinguen éxito de programa inválido.
- [X] Se liberan tokens, lexemas y estados aun cuando haya errores.
- [X] El proyecto no construye AST ni realiza validaciones semánticas.

## Pruebas

El proyecto contiene **17 pruebas públicas**. Para ejecutar todas:

```bash
make test
```

También puede ejecutarse directamente:

```bash
bash tests/public/run_public_tests.sh
```

El runner redirige la salida real a un archivo temporal y la compara con `diff -u`, de modo que la validación conserva y detecta diferencias de bytes como saltos de línea extra o caracteres `\r` inesperados.

En la revisión de esta entrega se obtuvo:

```text
Aprobadas: 17
Fallidas: 0
```

Consulte `tests/public/README.md` para el detalle de cada caso.

Además de que se puede verificar si hay fugas de memoria para alguno archivo de prueba
en el compilador usando la regla:

```bash
make memcheck FILE=<archivo.mc>
```

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

- Las pruebas automatizadas requieren Bash y `diff`; en Windows se debe usar WSL o un entorno tipo MSYS2/Git Bash que proporcione esas herramientas.
- La configuración de finales de línea depende de conservar el archivo `.gitattributes` al clonar o copiar el repositorio. Si los fixtures `.mc` son modificados por un editor que normaliza finales de línea, las pruebas específicas de CRLF pueden dejar de representar el caso original.
- El parser solo verifica que el programa tenga una derivación de la gramática correcta, sin verificar la semantica y el AST

## Notas de ejecución

- Verificado con `gcc -std=c11 -Wall -Wextra -Wpedantic` sin advertencias.
- Verificado en Linux con 13/13 pruebas exitosas.
- `make clean` elimina `build/` y el ejecutable generado.
- No se entregan binarios, archivos objeto ni contenido de `build/`
