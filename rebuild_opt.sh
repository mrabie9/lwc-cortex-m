#!/bin/sh
filename=$1
wdir=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )/Software
output_file=$filename.txt
echo $output_file
echo > $output_file # Overwrite once 

for app in schwaemm256128v2Armv7 schwaemm256256v2Armv7 romulusnOpt; do
	echo "=============================== $app ===============================" >> $output_file
	# echo "Cleaning $app"
	make clean -C $wdir/"$app"
	# echo "Building $app"
	make -C $wdir/"$app" >> $output_file
	echo >> $output_file
	echo >> $output_file
done

#$SHELL #(prevent shell from auto closing)