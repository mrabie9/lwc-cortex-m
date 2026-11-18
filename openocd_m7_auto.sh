#!/bin/bash
# Flash each benchmark ELF in turn; press '[' or '/' to program the next one.
# Uppdated Luciano Ost 02/11/23
#
# Usage: ./openocd_m7_auto.sh [opt]   (opt defaults to Os -> Build/<app>_Pwr_Os.elf)
# Env:   OPENOCD_SERIAL, OPENOCD  (see openocd_m7.sh)

root="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
opt="${1:-Os}"

for app in ascon128 ascon128Armv7 ascon128a ascon128aArmv7 isapa128v20 isapa128v20Armv7 isapa128av20 isapa128av20Armv7 schwaemm256128v2 schwaemm256128v2Armv7 schwaemm256256v2 schwaemm256256v2Armv7 tinyjambu tinyjambuOpt giftcofb128v1 xoodyak romulusn romulusnOpt elephant160v2 grain128aeadv2 photonbeetleaead128rate128v1; do
	elf_name="${app}_Pwr_${opt}"
	echo "=============================== ${elf_name}.elf ==============================="
	while true; do
		read -r -n 1 -t 10000 input
		if [[ $input == "[" || $input == "/" ]]; then
			"$root/openocd_m7.sh" "$elf_name" || echo "!! programming ${elf_name}.elf failed"
			break
		fi
	done
done
