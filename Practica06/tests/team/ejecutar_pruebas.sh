#!/usr/bin/env bash
# Pruebas creadas por el equipo: salida y diagnosticos esperados exactos.
set -u
set -o pipefail

base="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)" || exit 2
exe="${1:-./minic}"
if [[ ! -x "$exe" || ! -f "$exe" ]]; then
    printf 'No es un ejecutable accesible: %s\n' "$exe" >&2
    exit 2
fi
exe="$(cd -- "$(dirname -- "$exe")" && pwd)/$(basename -- "$exe")"
for cmd in timeout cmp mktemp diff; do
    command -v "$cmd" >/dev/null || { printf 'Falta %s\n' "$cmd" >&2; exit 2; }
done
limit="${TIMEOUT_S:-5}"
[[ "$limit" =~ ^[1-9][0-9]*$ ]] || { printf 'TIMEOUT_S debe ser entero positivo.\n' >&2; exit 2; }

tmp="$(mktemp -d)" || exit 2
trap 'rm -rf -- "$tmp"' EXIT
correctos=0
fallidos=0

while read -r id clase extra; do
    [[ -z "$id" || "$id" == \#* ]] && continue
    if [[ -n "${extra:-}" || ( "$clase" != valid && "$clase" != invalid ) ]]; then
        printf '[ERROR MANIFIESTO] Entrada incorrecta: %s %s %s\n' "$id" "$clase" "${extra:-}" >&2
        exit 2
    fi
    origen="$base/casos/$id.mc"
    esperado="$base/esperados/$id.$([[ "$clase" == valid ]] && echo out || echo err)"
    if [[ ! -f "$origen" || ! -f "$esperado" ]]; then
        printf '[ERROR ARCHIVOS] Faltan entrada o salida esperada de %s\n' "$id" >&2
        exit 2
    fi
    rm -f -- "$tmp/status"
    timeout -k 1s "${limit}s" bash -c '
        "$1" "$2" >"$3/out" 2>"$3/err"
        rc=$?
        printf "%s\n" "$rc" >"$3/status"
    ' _ "$exe" "$origen" "$tmp"
    if [[ ! -s "$tmp/status" ]]; then
        printf '[FALLO TEAM] %s (timeout o fallo al ejecutar)\n' "$id"
        fallidos=$((fallidos + 1))
        continue
    fi
    read -r rc < "$tmp/status"
    if [[ "$clase" == valid ]]; then
        if [[ "$rc" == 0 && ! -s "$tmp/err" ]] && cmp -s "$tmp/out" "$esperado"; then
            printf '[OK TEAM] %s\n' "$id"
            correctos=$((correctos + 1))
        else
            printf '[FALLO TEAM] %s: esperado valido; codigo=%s\n' "$id" "$rc"
            diff -u "$esperado" "$tmp/out" || :
            [[ ! -s "$tmp/err" ]] || { echo 'stderr inesperado:'; cat "$tmp/err"; }
            fallidos=$((fallidos + 1))
        fi
    else
        if [[ "$rc" != 0 && ! -s "$tmp/out" ]] && cmp -s "$tmp/err" "$esperado"; then
            printf '[OK TEAM] %s\n' "$id"
            correctos=$((correctos + 1))
        else
            printf '[FALLO TEAM] %s: esperado invalido; codigo=%s\n' "$id" "$rc"
            [[ ! -s "$tmp/out" ]] || { echo 'stdout inesperado:'; cat "$tmp/out"; }
            diff -u "$esperado" "$tmp/err" || :
            fallidos=$((fallidos + 1))
        fi
    fi
done < "$base/cases.txt"
printf '\nPruebas del equipo: %d correctas, %d fallidas.\n' "$correctos" "$fallidos"
((fallidos == 0))
