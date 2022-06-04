#!/bin/sh
if [ "$#" -ne 1 ]; then
    echo >&2 "Usage: $0 keymap"
    exit 1
fi
KEYMAPS_DIR="share/lotus/keymaps"
c="$(echo "$1" | cut -c1)"
install -dm 755 "$KEYMAPS_DIR/$c"
keymap/keymap "$1" > "$KEYMAPS_DIR/$c/$1"
