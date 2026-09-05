# Práctica 1 — Infraestructura básica del analizador léxico

## Información general

| Campo | Información |
|---|---|
| Asignatura | Compiladores |
| Número de práctica | 1 |
| Semestre | 2027-1 |

## Integrantes

| Nombre completo | Número de cuenta | Correo electrónico |
|---|---|---|
|Hermes Alberto Delgado Díaz | 319258613 | hermesdelgado@ciencias.unam.mx |
| | | |


## Estructura del proyecto

```text
Practica01_EquipoXX/
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
    ├── run_tests.sh
    ├── public/       (pruebas proporcionadas)
    └── equipo/       (pruebas propias del equipo)
```

### Módulos implementados

| Archivo o módulo | Responsabilidad |
|---|---|
| `src/main.c` | Punto de entrada: valida argumentos, abre el archivo fuente, invoca al analizador léxico y libera los recursos. |
| `include/lexer/token.h`, `src/lexer/token.c` | Definición del tipo `TokenType`, la estructura `Token` y las funciones para crear (`token_init`), imprimir (`token_print`) y liberar (`token_destroy`) un token, así como `token_type_name` para obtener el nombre textual de cada categoría. |
| `include/lexer/lexer.h`, `src/lexer/lexer.c` | Lógica del analizador léxico, construida directamente sobre la base proporcionada: `simple_token_type` (completada con todos los símbolos simples), `is_ignored_space` (sin cambios), `advance_position` (misma idea de la base, se le agregó el parámetro `file` para poder mirar el siguiente carácter y tratar `\r\n` como un único salto de línea) y `lexer_scan`, que sigue el mismo patrón de la base para emitir cada token (`token_init` → `token_print` → `token_destroy`) y ahora lo aplica también a `INTEGER`, `ERROR` y `TOKEN_EOF`. Se agregó únicamente la función `scan_integer`, necesaria porque un entero consume varios caracteres y la base no traía nada equivalente. |

## Requisitos

- GCC con soporte para C11.
- GNU Make.
- Dependencias adicionales: ninguna.

## Compilación

```text
make
```

Para eliminar los archivos generados:

```text
make clean
```

## Ejecución

```text
./minic programa.mc
```

Los tokens reconocidos se imprimen en `stdout`, uno por línea, con el
formato `línea:columna TIPO lexema` (sin lexema para `TOKEN_EOF`). Los
errores de uso (falta de argumento o archivo que no puede abrirse) se
imprimen en `stderr` y nunca se mezclan con la secuencia de tokens.


## Funcionalidades implementadas

- [X] Lectura del archivo fuente.
- [X] Sistema de tokens.
- [X] Seguimiento de línea y columna.
- [X] Manejo de espacios en blanco.
- [X] Símbolos simples.
- [X] Números enteros.
- [X] Caracteres no reconocidos mediante `ERROR`.
- [X] Final del archivo mediante `TOKEN_EOF`.

## Pruebas

La práctica incluye dos conjuntos de pruebas basadas en comparación exacta de salida estándar contra un archivo `.out` esperado (ver `tests/README.md` para el detalle de cada caso):

- `tests/public/`: pruebas proporcionadas.
- `tests/team/`: pruebas adicionales del equipo (un dígito, varios caracteres no reconocidos consecutivos, combinación de tabuladores y espacios alrededor de tokens de más de un carácter, y delimitadores anidados).

Para ejecutarlas todas:

```text
make test
```

## Problemas conocidos

> No se conocen problemas adicionales a las limitaciones propias del alcance de la Práctica 1.

## Notas de ejecución

- El proyecto se probó con `gcc -std=c11 -Wall -Wextra -Wpedantic`, sin
  advertencias.
- `make clean` elimina el directorio `build/` y el ejecutable `minic`; no
  se entregan binarios ni archivos objeto.
