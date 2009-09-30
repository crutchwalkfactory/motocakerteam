#!/bin/bash
source ezxenv.sh
make && make install && ncftpput -u root -p "" 192.168.16.2 /mmc/mmca1/libs /arm-eabi/lib/libSDL-1.2.so.0
