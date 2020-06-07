import sys
from signal import signal, SIGINT
from pyBusPirateLite.SPI import SPI

run = [True]

def signal_handler(sig, frame):
    print('You pressed Ctrl+C!')
    run[0] = False

signal(SIGINT, signal_handler)

spi = SPI('/dev/ttyUSB0')
spi.pins = SPI.PIN_POWER | SPI.PIN_CS 
spi.config = SPI.CFG_PUSH_PULL 
spi.speed = '125kHz'

# send two bytes and receive answer
while(run[0]):
    spi.cs = True
    data = spi.transfer(list(b'Por lo demas'))
    spi.cs = False