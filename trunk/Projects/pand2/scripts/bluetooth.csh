#!/bin/sh
if [ ! -f /tmp/BT.on ]
then
/bin/sh /etc/initservices/services/bluetooth start
/usr/SYSqtapp/bluetooth/bluetooth &
sleep 3
else
/bin/sh /etc/initservices/services/bluetooth stop
fi
/bin/kill `pidof bluetooth`