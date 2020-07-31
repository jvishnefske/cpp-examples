""" program to read from serial port, and writ rot13 version back."""
import serial
import logging
logger = logging.getLogger("serialPortEcho")

def rot13(phrase):
   abc = "abcdefghijklmnopqrstuvwxyz"
   out_phrase = ""
   for char in phrase:
       out_phrase += abc[(abc.find(char)+13)%26]
   return out_phrase


def main():
	port='/dev/ttyS1'
	speed=38400
	with serial.Serial(port, speed) as ser:
		_=" "
		while len(_)>0:
			_ = ser.read(100)
		ser.write(rot13(_))
		
	
if __name__ == "__main__":
	main()
