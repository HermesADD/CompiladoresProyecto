#!/usr/bin/env bash

set -u

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
BIN="$ROOT_DIR/minic"

if [ ! -x "$BIN" ]; then
    echo "No se encontró el ejecutable '$BIN'. Ejecute 'make' primero." >&2
    exit 2
fi

pass=0
fail=0

run_suite() {
    local suite_dir="$1"
    local inputs_dir="$suite_dir/inputs"

    [ -d "$inputs_dir" ] || return 0

    for input in "$inputs_dir"/*.mc; do
        [ -e "$input" ] || continue

        local base
        base="$(basename "$input" .mc)"
        local expected="$suite_dir/expected/$base.out"
        local actual
        actual="$("$BIN" "$input" 2>/tmp/minic_test_stderr)"

        if [ ! -f "$expected" ]; then
            echo "SIN ARCHIVO ESPERADO: $input"
            fail=$((fail + 1))
            continue
        fi

        if diff -u "$expected" <(printf '%s\n' "$actual") > /tmp/minic_test_diff; then
            echo "PASA: $base"
            pass=$((pass + 1))
        else
            echo "FALLA: $base"
            cat /tmp/minic_test_diff
            fail=$((fail + 1))
        fi
    done
}

run_suite "$ROOT_DIR/tests/public"
run_suite "$ROOT_DIR/tests/team"

echo ""
echo "Resultado: $pass pruebas exitosas, $fail pruebas fallidas."

if [ "$fail" -ne 0 ]; then
    exit 1
fi