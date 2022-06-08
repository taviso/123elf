#!/bin/sh
gnu_make="$(command -v make 2>/dev/null)"
if command -v gmake >/dev/null 2>&1; then
    gnu_make="$(command -v gmake)"
fi
if [ -z "$gnu_make" -o "$("$gnu_make" --version 2>/dev/null | head -n 1 | cut -c-3)" != "GNU" ]; then
    echo >&2 "Could not find GNU make in PATH"
    exit 1
fi
