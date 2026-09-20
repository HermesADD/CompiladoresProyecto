# Changelog

Este proyecto sigue un progreso por práctica.

## [1.0.0] - Práctica  2

### Añadido

Se agrego en el lexer la lectura de identificadores, palabras reservadas, operadores compuestos y
además de agregar el caso de uso de comentarios de una sola línea y el manejo de
la posición del lexema.

### Corregido

Se modifico la función `simple_token_type` la cual elimina los casos de las operaciones
que comparten caracteres con la operaciones compuestas, lo cual se maneja
en la función `evaluate_compound_operator`

### Documentación

- README ampliado con descripción del proyecto, módulos, compilación, ejecución, funcionalidades y pruebas.
- Instrucciones separadas para Linux/WSL y Windows con MSYS2/MinGW64.
- Sección específica sobre problemas de portabilidad entre CRLF (`\r\n`) y LF (`\n`).
- Explicación del efecto de `core.autocrlf`/normalización de Git sobre los archivos de pruebas.
- Documentación de la necesidad de conservar `run_public_tests.sh` con LF para evitar problemas de ejecución en Linux.
- Limitaciones de las pruebas automatizadas desde PowerShell/CMD documentadas; se recomienda WSL o un entorno Bash compatible.