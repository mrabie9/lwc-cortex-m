# Uppdated Luciano Ost 02/11/23
#!/bin/bash

ocddir="C:/OpenOCD/share/openocd/scripts"
app_name="$1" # pass app name on CLI
wdir="C:/Git/lwc-cortex-m/Build" # directory of project folder
elf_file="$app_name.elf"

echo
openocd -f C:/Git/lwc-cortex-m/Board/board.cfg -f C:/Git/lwc-cortex-m/Board/stm32f7discovery.cfg -c "program $wdir/$elf_file verify reset exit" #-c "reset init"

echo
#$SHELL #(prevent shell from auto closing)