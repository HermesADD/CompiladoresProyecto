# Práctica 6 — MiniC

## Descripción

Esta práctica **extiende la Práctica 5**. Se reutilizan su tabla de símbolos, ámbitos anidados, resolución de identificadores y detección de redeclaraciones y *shadowing*. Se incorpora la **verificación estática de tipos** sobre el AST heredado de la Práctica 4. El lexer y parser de etapas anteriores continúan disponibles; `parser_parse_ast` en `parser_ast.c` construye el AST que analiza la etapa semántica.

El analizador valida tipos `int` y `bool` en declaraciones, asignaciones, expresiones aritméticas, relacionales, lógicas, de igualdad y condiciones de `if` y `while`. El tipo `TYPE_ERROR` permite evitar diagnósticos derivados de otro error. No se realizan conversiones implícitas ni ejecución del programa fuente.

## Compilación y ejecución

Desde la carpeta `Practica06`:

```sh
make clean && make
./minic tests/public/casos/p22_completo_valido.mc
./minic tests/team/casos/t06_shadowing_con_tipos.mc
```

Si el archivo es válido, la salida normal es exclusivamente:

```text
Programa semanticamente correcto.
```

Un programa inválido escribe los diagnósticos en `stderr`, deja `stdout` vacío y termina con un código distinto de cero. La opción `--ast` permite consultar el AST para depuración:

```sh
./minic --ast tests/team/casos/t02_precedencia_aritmetica.mc
```

## Arquitectura modular

```text
include/semantic/          src/semantic/
  semantic.h                 semantic.c
  symbol.h                   symbol.c
  scope.h                    scope.c
  symbol_table.h             symbol_table.c
  type.h                     type.c
  type_checker.h             type_checker.c
```

- `symbol`: almacena nombre, tipo y declaración de cada identificador.
- `scope`: administra símbolos dentro de un ámbito.
- `symbol_table`: administra ámbitos, búsquedas locales/visibles e inserciones.
- `semantic`: recorre el AST y coordina declaraciones, usos y diagnósticos.
- `type`: representa los tipos y sus nombres.
- `type_checker`: valida tipos y operadores en expresiones y sentencias.

Los nodos AST poseen sus hijos; las referencias a símbolos y declaraciones no transfieren su propiedad. La tabla conserva los ámbitos retirados hasta la destrucción del analizador. El tipo resultante se devuelve desde el recorrido de expresiones, sin añadirse un campo al AST.

## Pruebas

Se mantienen **dos conjuntos independientes** dentro de `tests/`:

```text
tests/
├── README.md
├── public/                 # material original del profesor (24 casos)
│   ├── casos/
│   ├── esperados/
│   ├── cases.txt
│   ├── ejecutar_pruebas.sh
│   └── README.md
└── team/                   # pruebas adicionales del equipo (20 casos)
    ├── casos/
    ├── esperados/
    ├── cases.txt
    ├── ejecutar_pruebas.sh
    ├── DESCRIPCION.md
    └── README.md
```

### Comandos

```sh
make test-public   # 24 pruebas oficiales
make test-team     # 20 pruebas del equipo
make test          # 44 pruebas en total
make test-all      # equivalente a make test
```

Las pruebas públicas mantienen su ejecutor original: verifican el código de salida y el contrato general de stdout/stderr, con diagnósticos inválidos sujetos a revisión humana. Las pruebas del equipo comparan **exactamente** stdout/stderr con los archivos de `tests/team/esperados/`, incluyendo la cantidad, el contenido y las posiciones de los errores reportados.

Los 20 casos del equipo cubren, entre otros, precedencia, asociatividad, booleanos, ámbitos, *shadowing*, inicializaciones, asignaciones, tipos incompatibles, `TYPE_ERROR` y recuperación tras **dos errores sintácticos**. Consulta `tests/team/DESCRIPCION.md` para sus objetivos individuales.

## Memoria

Para revisar fugas y errores de memoria con Valgrind:

```sh
make memcheck FILE=tests/public/casos/p22_completo_valido.mc
make memcheck FILE=tests/team/casos/t06_shadowing_con_tipos.mc
```

En un caso inválido Valgrind puede reportar ausencia de errores de memoria y, aun así, el proceso `make` terminar con código no cero porque MiniC rechazó la entrada. Son comprobaciones distintas.

## Limitaciones

El análisis es estático: no ejecuta MiniC, no infiere declaraciones, no hace conversiones implícitas entre `int` y `bool`, no optimiza ni genera código objeto. Los tests no reemplazan pruebas privadas ni una revisión general de memoria. El conjunto `team` documenta el comportamiento actual esperado del compilador y facilita detectar regresiones.
