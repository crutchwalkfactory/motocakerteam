#!/bin/sh
PATH=$PATH:/mnt/msc_int0/bin
showRadio "Меню выключения" "Выберите действие:" "Перезагрузить" "Выключить" "Соединения" "Менеджер загрузок" "Bluetoot Вкл/Выкл"
RADIORES=$?
if [ $RADIORES -eq 0 ]
then
	exit

elif [ $RADIORES -eq 1 ]
then	
	/sbin/reboot
	showNotify "" "Телефон будет перезагружен, пожалуйста ждите" 1 10

elif [ $RADIORES -eq 2 ]
then	
	/sbin/poweroff
	showNotify "" "Телефон будет выключен, пожалуйста ждите" 1 10


elif [ $RADIORES -eq 3 ]
then	
	exec /usr/SYSqtapp/systemsetup/connectionsetup

elif [ $RADIORES -eq 4 ]
then	
	exec /usr/SYSqtapp/downloadmanager/downloadmanager

elif [ $RADIORES -eq 5 ]
then	
	if [ ! -f /tmp/BT.on ]
	then
	/bin/sh /etc/initservices/services/bluetooth start
	/usr/SYSqtapp/bluetooth/bluetooth &
	sleep 3
	else
	/bin/sh /etc/initservices/services/bluetooth stop
	fi
	/bin/kill `pidof bluetooth`

fi