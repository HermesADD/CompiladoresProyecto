# Pruebas del equipo (20 casos)

Estas pruebas fueron elaboradas por el equipo y no forman parte del material oficial del profesor.

| Caso | Resultado esperado | Propósito |
|---|---|---|
| `t01_programa_vacio` | Válido | Programa vacio y comentario |
| `t02_precedencia_aritmetica` | Válido | Precedencia aritmetica |
| `t03_asociatividad_y_unarios` | Válido | Asociatividad izquierda y negativos anidados |
| `t04_booleanos_anidados` | Válido | Operadores booleanos, igualdad y parentesis |
| `t05_ambitos_anidados` | Válido | Resolucion de identificadores en ambitos anidados |
| `t06_shadowing_con_tipos` | Válido | Shadowing entre int y bool |
| `t07_if_else_y_while` | Válido | Ramas y ciclo con condiciones booleanas |
| `t08_autorreferencia_permitida` | Válido | Identificador visible durante su propia inicializacion |
| `t09_uso_antes_de_declarar` | Inválido | Uso de identificador previo a declaracion |
| `t10_fuera_de_ambito` | Inválido | Uso fuera de ambito |
| `t11_redeclaracion_y_conservacion` | Inválido | Redeclaracion conserva simbolo original |
| `t12_inicializaciones_incompatibles` | Inválido | Dos inicializaciones incompatibles |
| `t13_asignaciones_incompatibles` | Inválido | Dos asignaciones incompatibles |
| `t14_aritmetica_y_unario_invalidos` | Inválido | Suma incorrecta y negativo unario incorrecto |
| `t15_relacionales_invalidos` | Inválido | Operandos invalidos para operadores relacionales |
| `t16_igualdad_tipos_distintos` | Inválido | Igualdad requiere tipos iguales |
| `t17_logicos_invalidos` | Inválido | Operadores logicos requieren booleanos |
| `t18_condiciones_no_booleanas` | Inválido | Condiciones if y while requieren bool |
| `t19_propagacion_sin_cascada` | Inválido | Dos errores independientes sin errores en cascada |
| `t20_recuperacion_sintactica` | Inválido | Dos errores sintacticos independientes tras recuperacion |
