import spidev
import time

spi = spidev.SpiDev() 
spi.open(0, 1)
spi.mode = 3
spi.cshigh = False

L = [191, 192, 129]
resp = [ ]
resp = spi.xfer(L)

spi.close()

zAccel = (L[1] << 8) + L[2]
print(zAccel)
