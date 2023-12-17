import serial
import struct
import sys
import subprocess
import os
import time
import signal
import random
import re
from datetime import datetime
import platform
import logging
import traceback

algorithm = "hello_world"
exec_name = algorithm + ".elf"
#sn, serial_port = ("066CFF495257808667092543", "/dev/ttyACM0") # TODO: Check
sn, serial_port = ("0669FF555187534867152037", "COM5") # TODO: Check
serial_port = "COM5" # 0669FF555187534867152037


def clearBuffer():
	max_n_attempts = 50

	n_attempts = 1

	while (n_attempts <= max_n_attempts):
		a = nucleo.read(4)
		print(a)
		if (a == b''):
			print("Serial buffer is clean")
			break
		n_attempts += 1

	max_n_attempts += 1

	if n_attempts == max_n_attempts:
		print("Serial buffer is still not clean")
		register_log_full.close()
		register_log_clean.close()
		print(quit)
		os._exit(17)

	nucleo.flushInput()
	nucleo.flushOutput()

def start_board():
	global countdown_to_reset

	prog_log = subprocess.run(r"openocd.sh " + algorithm, shell=True, capture_output=True, text=True) # TODO: review
	print("openOCD")
	print(prog_log.stdout,'\n',prog_log.stderr)

	if "Error" in prog_log.stdout or "Error" in prog_log.stderr or "failed" in prog_log.stdout or "failed" in prog_log.stderr:
		print("OpenOCD failed to program the board!")
		os._exit(10)
	clearBuffer()
	time.sleep(.2)


try:
	nucleo = serial.Serial(serial_port, 115200, bytesize=serial.EIGHTBITS, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, timeout = 5)
	print("Nucleo\n")
except Exception as e:
	print("Problem opening serial port:\n", e)
	exit()

start_board()

msg = 0.0

def float_to_hex(f):
    return struct.pack('<f', f)

def hex_to_float(f):
	# check if data has 4 bytes, needed by unpack
	if len(f) == 4:
		return struct.unpack("<f" ,f)[0]
	return 999 # TODO: error
	
while(1):
	try:
		print("Sending: ", msg, end=' - ')
		nucleo.write(float_to_hex(msg))
		message = nucleo.read(4)
		print("Received: ", hex_to_float(message), end='\r')
		msg += 1
		if msg > 10:
			break
		time.sleep(1)
	except Exception as e:
		print("Shouldnt be here->", e)
		exit()

print("\nBoard Working!")
