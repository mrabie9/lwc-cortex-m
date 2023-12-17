		print("Sending: ", msg)
		nucleo.write(float_to_hex(msg))
		enc_data.append(nucleo.read(4))
		print("Received Enc: ", hex_to_int(enc_data[idx]))
		dec_data.append(nucleo.read(4))
		# print("Received Dec: ", hex_to_int(dec_data[idx]), end='\r')
		idx = idx + 1
		msg += 1
		if msg > 10:
			print("Received Enc: ", enc_data)
			print("Received Dec: ", enc_data)
			break
		time.sleep(1)