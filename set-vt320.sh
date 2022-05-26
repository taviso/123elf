#!/bin/bash

# Key redefinition for DEC VT320
# for changing multiple-sheets file
# use SHIFT+F19 and SHIFT+20 after launching this script

# Redefine Shift+F19 (VT320) as Ctrl+PgUp (PC)
echo -e "\eP1;1;1|33/1b5b353b357e\e\\"

# Redefine Shift+F20 (VT320) as Ctrl+PgDn (PC)
echo -e "\eP1;1;1|34/1b5b363b357e\e\\"
