#
# set to hardware used to exec code on your snes
# 
#USE_SWC=1
USE_QD16 = 1
USE_QD16_IPS = 1

#
# parallel and serial port specifications
# 
PARPORT=ccc8
SERPORT=/dev/ttyUSB0

#
# linker script
#
OUTPUT=smc

#
# misc
#
QDIPS_ROM_FILE = .rom
QDIPS_IPS_FILE = .patch
