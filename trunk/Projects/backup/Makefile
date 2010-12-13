TOOLPREFIX:=	/arm-eabi
QTDIR	:=	$(TOOLPREFIX)/lib/qt-2.3.8
EZXDIR	:=	$(TOOLPREFIX)/lib/ezx-z6

CC	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-gcc
CXX	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-g++
LD	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-g++
CFLAGS	=	-pipe -Wall -W -O2 -DNO_DEBUG $(INCPATH)
CXXFLAGS=	-pipe -DQWS -fno-exceptions -fno-rtti -Wall -W -O2 -DNO_DEBUG $(INCPATH)
INCPATH	=	-I$(QTDIR)/include -I$(EZXDIR)/include
LDFLAGS	=	-s
LDLIBS	=	$(SUBLIBS) -L$(TOOLPREFIX)/arm-linux-gnueabi/lib -L$(QTDIR)/lib -L$(EZXDIR)/lib -lm -lqte-mt -lezxpm -ljpeg -lezxappbase -lezxtapi-xscale-r -llog_util -llighting -lezxam -lezxphone -lezxappsdk -lezxmsgctrviewutility -lezxmessagingsdk -lezxdb2 -ldmnative -lezxdrmsp -lezxdrmclibhack -lezxdrmsecuretime -lezxdrmcipher -lhackfs -lzcopchannel -lezxalert -lezxidle -lezxalarmagent -lezxalarmclock -lezxquickalarmapi -lezxalarmserver -lezxbluetooth -lezxmbtapi
MOC	=	$(QTDIR)/bin/moc

TARGETS = back

all: $(TARGETS)

back: back.cpp

clean:
	- rm back
