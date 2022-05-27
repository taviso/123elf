#!/bin/bash

if [ "${TERM:0:2}" == "vt" ]; then

  # Redefine Shift+F20 (VT320) as Ctrl+PgUp (PC)
  echo -n -e "\eP1;1;1|34/1b5b353b357e\e\\"

  # Redefine Shift+F19 (VT320) as Ctrl+PgDn (PC)
  echo -n -e "\eP1;1;1|33/1b5b363b357e\e\\"

  # 33 is "F19"
  # 34 is "F20"

  # Redefine Shift+F6/F11 (VT320) as ESC (PC)
  echo -n -e "\eP1;1;1|17/1b\e\\"
  echo -n -e "\eP1;1;1|23/1b\e\\"


fi

