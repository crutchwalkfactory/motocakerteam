TOOLPREFIX:=	/arm-eabi
QTDIR	:=	$(TOOLPREFIX)/lib/qt-2.3.8
EZXDIR	:=	$(TOOLPREFIX)/lib/ezx-z6

CC	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-gcc
CXX	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-g++
LD	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-g++
CFLAGS	=	-pipe -Wall -W -O2 -DNO_DEBUG $(INCPATH)
CXXFLAGS=	-pipe -DQWS -fno-exceptions -fno-rtti -Wall -W -O2 -DNO_DEBUG $(INCPATH)
INCPATH	=	-I. -I./include -I$(QTDIR)/include -I$(EZXDIR)/include
LDFLAGS	=	-D$(VERSION) -s
LDLIBS	=	-L$(QTDIR)/lib -L$(EZXDIR)/lib -L$(TOOLPREFIX)/lib -L$(EZXDIR)/lib/ezx/lib -lm -lbluetooth -lqte-mt -lezxpm -lezxappbase -lezxtapi-xscale-r -llog_util -llighting
MOC	=	$(QTDIR)/bin/moc
RM=rm -f
MV=mv

STRIP	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-strip

UPLOAD_PATH = /mmc/mmca1/.pol/bin/

TARGETS = mp32rtp
SRC = mp32rtp.c

OBJ = $(SRC:.c=.o)

all: $(TARGETS)

mp32rtp: $(OBJ)
	$(CC) $(LDFLAGS) $(LDLIBS) -o $@ $^

.c.o:

install: all
	@echo Install ncftp package, connect phone to computer
	ncftpput -u root -p "" 192.168.16.2 $(UPLOAD_PATH) $(TARGETS)

clean:
	$(RM) $(TARGETS)
	$(RM) *.o

