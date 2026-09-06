# Pruebas 

Este directorio contiene dos conjuntos de pruebas:

- `public/`: pruebas públicas proporcionadas junto con el enunciado de la
  práctica (no modificadas).
- `team/`: pruebas adicionales creadas por el equipo para cubrir casos
  no contemplados explícitamente por las pruebas públicas.

## Ejecución

Con el proyecto ya compilado (`make`), puede ejecutarse todo el conjunto de pruebas con:

```text
make test
```

o directamente:
```text
bash tests/run_tests.sh
```

El script compara, para cada archivo `inputs/<caso>.mc`, la salida estándar
producida por `./minic` contra el archivo `expected/<caso>.out` correspondiente.
Un caso se considera exitoso únicamente si la salida coincide de forma exacta.

## Casos propios (`team/`)

| Caso | Propósito principal |
|---|---|
| `t1_single_digit.mc` | Un entero de un solo dígito (`5`), incluyendo el caso límite del dígito cero. |
| `t2_multiple_errors.mc` | Varios caracteres no reconocidos (`\|`, `@`) en la misma línea, verificando que el análisis se recupere después de cada uno. |
| `t3_tabs.mc` | Tabuladores y espacios combinados alrededor de tokens de más de un carácter (enteros), y una línea final terminada en salto de línea tras un tabulador. |
| `t4_consecutive_delimiters.mc` | Delimitadores anidados (`(`, `)`) junto con un entero y `;`, para confirmar el reconocimiento correcto de símbolos simples consecutivos. |

Estas pruebas son un complemento de las pruebas públicas.