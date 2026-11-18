#!/bin/bash
# Flash an ELF onto the STM32F7 board with OpenOCD.
# Uppdated Luciano Ost 02/11/23
#
# Usage: ./openocd_m7.sh <app_name | path/to/file.elf>
#   app_name -> Build/<app_name>.elf  (e.g. ascon128_Pwr_Os)
#
# Env:
#   OPENOCD_SERIAL  ST-LINK serial to use (only needed with several boards attached)
#   OPENOCD         openocd executable (default: openocd on PATH)

set -e

root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
openocd_bin="${OPENOCD:-openocd}"

if [[ -z "$1" ]]; then
	echo "usage: $0 <app_name | file.elf>" >&2
	exit 1
fi

if [[ "$1" == *.elf ]]; then
	elf="$1"
else
	elf="$root/Build/$1.elf"
fi

if [[ ! -f "$elf" ]]; then
	echo "error: ELF not found: $elf" >&2
	exit 1
fi

serial_cmd=()
[[ -n "$OPENOCD_SERIAL" ]] && serial_cmd=(-c "adapter serial $OPENOCD_SERIAL")

echo
"$openocd_bin" -s "$root/Board" -f "$root/Board/stm32f7discovery.cfg" "${serial_cmd[@]}" \
	-c "program {$elf} verify reset exit"
echo
