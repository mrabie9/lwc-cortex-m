#!/bin/bash
# Load and run an ELF on the NUCLEO-N657X0-Q (STM32N657, Cortex-M55).
#
# The STM32N6 has no internal flash: the benchmark is linked into AXISRAM2
# (Board/STM32N657X0HXQ_AXISRAM2_fsbl.ld), loaded over SWD and started from
# its vector table. Put the board in development boot mode first:
#   BOOT1 switch = position 2-3 (BOOT0 don't care), then power-cycle / reset.
# The program stays in RAM, so it is lost on power-off and must be reloaded.
#
# Usage: ./openocd_m55.sh <app_name | path/to/file.elf>
#   app_name -> Build/m55/<app_name>.elf  (e.g. ascon128_Pwr_Os)
#
# Env:
#   OPENOCD_SERIAL  ST-LINK serial to use (only needed with several boards attached)
#   STM32_PROG      STM32_Programmer_CLI executable (default: on PATH)
#
# Uses STM32CubeProgrammer rather than OpenOCD: the OpenOCD build installed
# here has no STM32N6 target. The script name matches openocd_<board>.sh so
# lwc_performance.py can call it with board = "m55".

set -e

root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
prog="${STM32_PROG:-STM32_Programmer_CLI}"

# Start address = vector table (initial MSP, then Reset_Handler) = ORIGIN(ROM)
vtor=0x34180400

if [[ -z "$1" ]]; then
	echo "usage: $0 <app_name | file.elf>" >&2
	exit 1
fi

if [[ "$1" == *.elf ]]; then
	elf="$1"
else
	elf="$root/Build/m55/$1.elf"
fi

if [[ ! -f "$elf" ]]; then
	echo "error: ELF not found: $elf" >&2
	exit 1
fi

sn=()
[[ -n "$OPENOCD_SERIAL" ]] && sn=("sn=$OPENOCD_SERIAL")

echo
# AP=1 is the Cortex-M55 access port on STM32N6.
"$prog" -c port=SWD mode=HOTPLUG AP=1 "${sn[@]}" -d "$elf" -s "$vtor"
echo
