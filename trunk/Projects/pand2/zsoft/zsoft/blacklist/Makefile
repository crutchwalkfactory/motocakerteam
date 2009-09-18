TOOLPREFIX:= /arm-eabi
QTDIR	:=	$(TOOLPREFIX)/lib/qt-2.3.8
EZXDIR	:=	$(TOOLPREFIX)/lib/ezx-z6
MOTODIR :=	/home/matias/Motorola/MOTODEV_Studio_for_Linux_v0.3.0/MOTOMAGX_0.3_SDK
CC	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-gcc
CXX	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-g++
LD	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-g++
CFLAGS	=	-pipe -Wall -W -O2 -DNO_DEBUG $(INCPATH)
CXXFLAGS=	-pipe -DQWS -fexceptions -fno-rtti -Wall -W -O2 -DNO_DEBUG $(INCPATH)
INCPATH	=	-I$(QTDIR)/include2 -I$(EZXDIR)/include2 -I$(MOTODIR)/include -I$(MOTODIR)/include/linux/x86
LDFLAGS	=	-s
LDLIBS	=	-L$(QTDIR)/lib -L$(EZXDIR)/lib -lm -lqte-mt -lezxpm -ljpeg -lezxappbase -lezxtapi-xscale-r -llog_util -llighting
MOC	=	$(QTDIR)/bin/moc

TARGETS = blacklist

all: $(TARGETS)

blacklist: blacklist.cpp


