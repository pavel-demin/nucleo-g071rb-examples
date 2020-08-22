import sys
import struct
import Adafruit_PureIO.smbus as smbus

try:
  value = int(sys.argv[1])
  assert(0 <= value <= 1)
except:
  print('Usage: test.py 0|1')
  sys.exit(1)

address = 0x55

bus = smbus.SMBus(3)

if value == 1:
  bus.write_bytes(address, struct.pack('I', 0x12345678))
else:
  bus.write_bytes(address, struct.pack('I', 0x87654321))
