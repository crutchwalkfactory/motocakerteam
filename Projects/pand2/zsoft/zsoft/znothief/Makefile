TOOLPREFIX:=	/arm-eabi
QTDIR	:=	$(TOOLPREFIX)/lib/qt-2.3.8
EZXDIR	:=	$(TOOLPREFIX)/lib/ezx-z6

CC	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-gcc
CXX	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-g++
LD	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-g++
CFLAGS	=	-pipe -Wall -W -O2 -DNO_DEBUG $(INCPATH)
CXXFLAGS=	-pipe -DQWS -fno-exceptions -fno-rtti -Wall -W -O2 -DNO_DEBUG $(INCPATH)
INCPATH	=	-I$(QTDIR)/include -I$(EZXDIR)/include -I/home/jesus/Motorola/MOTODEV_Studio_for_Linux_v0.3.0/MOTOMAGX_0.3_SDK/include/usadas
LDFLAGS	=	-s
LDLIBS	=	-L$(QTDIR)/lib -L$(EZXDIR)/lib -lezxtapi-xscale-r -llog_util -lqte-mt
MOC	=	$(QTDIR)/bin/moc

TARGETS = sendsms \
	  getimsi \
	  zNothief \



all: $(TARGETS)
sendsms: sendsms.cpp
getimsi: getimsi.cpp
zNothief: zNothief.cpp

