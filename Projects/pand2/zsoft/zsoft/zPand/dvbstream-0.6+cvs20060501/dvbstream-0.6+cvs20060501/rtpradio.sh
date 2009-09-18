#!/bin/sh
DUMPRTP=dumprtp
#TS2ES is in the dvb-mpegtools package
TS2ES=ts2es
# MPG123 is in the mpg123 package
MPG123='mpg123 -'


$DUMPRTP | $TS2ES 2 | $MPG123
