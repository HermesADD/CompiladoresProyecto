# Pruebas de MiniC — Práctica 6

Se distinguen **dos conjuntos** que se ejecutan contra el mismo compilador:

| Carpeta | Origen | Casos | Tipo de validación |
|---|---|---:|---|
| `public/` | Profesor; contenido conservado sin modificar | 24 | Contrato de aceptación/rechazo, `stdout`, `stderr` y estado |
| `team/` | Casos nuevos elaborados por el equipo | 20 | Comparación **exacta** de `stdout` o diagnósticos `stderr`, más estado |

## Estructura

```text
tests/
├── README.md
├── public/
│   ├── README.md
│   ├── cases.txt
│   ├── casos/                  # 24 archivos originales
│   ├── esperados/
│   │   ├── CASOS.md
│   │   └── programa_correcto.out
│   └── ejecutar_pruebas.sh
└── team/
    ├── README.md
    ├── DESCRIPCION.md         # propósito de cada prueba
    ├── cases.txt
    ├── casos/                 # 20 programas adicionales
    ├── esperados/             # una salida exacta por caso (.out o .err)
    └── ejecutar_pruebas.sh
```

## Ejecución desde `Practica06/`

```sh
make clean && make
make test-public  # 24 pruebas originales del profesor
make test-team    # 20 pruebas nuevas del equipo
make test        # ambas baterías (44 casos)
```

También puede ejecutarse `make test-all` para ambas baterías.

La prueba `t20_recuperacion_sintactica.mc` comprueba que se reporten **dos** errores sintácticos en distintas líneas, como en la corrección aplicada a `parser_ast.c`.

**Importante:** las pruebas públicas se preservaron tal como fueron entregadas. Su script no exige coincidencia textual de los diagnósticos de error. En cambio, las pruebas `team` son más estrictas: comprueban texto, posiciones y cantidad de diagnósticos mediante comparación completa de `stderr`.

Las pruebas no sustituyen validaciones adicionales de memoria ni las pruebas privadas del profesor.
