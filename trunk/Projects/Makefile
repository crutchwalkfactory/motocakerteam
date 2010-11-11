#TOOLPREFIX:=	/home/abram/z6/arm-eabi
TOOLPREFIX:=	/arm-eabi
#Remember to set your own TOOLPREFIX. A to pi3dec bydet!
QTDIR	:=	$(TOOLPREFIX)/lib/qt-2.3.8
EZXDIR	:=	$(TOOLPREFIX)/lib/ezx-z6
UPLOAD_PATH :=  /mmc/mmca1/bin/
UPLOAD_PATH2 :=  /ezxlocal/download/appwrite/setup/

CC	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-gcc
CXX	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-g++
LD	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-g++
CFLAGS	=	-pipe -Wall -W -g $(INCPATH)
CXXFLAGS=	-pipe -DQWS -fno-exceptions -fno-rtti -Wall -W -g $(INCPATH)
#INCPATH	=	-I$(QTDIR)/include
INCPATH	=	-I$(QTDIR)/include -I$(EZXDIR)/include -I $(TOOLPREFIX)/arm-linux-gnueabi/include
LDFLAGS	=	
#LDLIBS	=	-L$(TOOLPREFIX)/arm-linux-gnueabi/lib -L$(QTDIR)/lib -lqte-mt -lezxappbase
LDLIBS = 	-L$(TOOLPREFIX)/arm-linux-gnueabi/lib -L$(QTDIR)/lib -L$(EZXDIR)/lib -lqte-mt
#SOLDLIBS = 	-L$(TOOLPREFIX)/arm-linux-gnueabi/lib -L$(QTDIR)/lib -L$(EZXDIR)/lib -lm -lqte-mt -lezxpm -ljpeg -lezxappbase -lezxtapi-xscale-r -llog_util -llighting 
MOC	=	$(QTDIR)/bin/moc
RM=rm -f
MV=mv

TARGETS = libezxvibrate.so

all: $(TARGETS)

libezxvibrate.so: libezxvibrate.o
	$(LD) $(LDFLAGS) $(LDLIBS) -shared -o $@ $^
#	$(LD) $(LDFLAGS) $(SOLDLIBS) -shared -o $@ $^

libezxvibrate.o: libezxvibrate.cpp

install: $(TARGETS)
	ncftpput -u root -p "" 192.168.16.2 $(UPLOAD_PATH) $(TARGETS)
	ncftpput -u root -p "" 192.168.16.2 $(UPLOAD_PATH2) $(ADDFILES)	

    
clean:
	$(RM) $(TARGETS)
	$(RM) moc_*.cpp
	$(RM) *.o
	$(RM) *.so
	$(RM) *~

