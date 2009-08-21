
### Variables ###
DESTDIR=../dist/ # might be handy for distro peeps
#DESTDIR=/ 
#PREFIX=/usr/local
#PREFIX=./cdogsdata
#BINDIR=$(PREFIX)/games/bin/
#DATADIR=$(PREFIX)/share/games/cdogs/
DATADIR = ./data/
#DATADIR=./ # for windows
#PREFIX=./  # or self contained install
TOOLPREFIX:=	/arm-eabi

QTDIR	:=	$(TOOLPREFIX)/lib/qt-2.3.8
EZXDIR	:=	$(TOOLPREFIX)/lib/ezx-z6

CC=/arm-eabi/bin/arm-linux-gcc
STRIP=/arm-eabi/bin/arm-linux-strip

#CFLAGS=-O2 -Wall `sdl-config --cflags` -std=c99 -funroll-loops
CF_OPTIMISE=-O2 
#-msoft-float
CF_DEBUG= -Wall -ggdb
CFLAGS=${CF_OPTIMISE} ${CF_DEBUG} `/arm-eabi/SDL/bin/sdl-config --cflags` -I/arm-eabi/include

INCLUDES=-I./include -I./missions
LDFLAGS= -lSDL_mixer `/arm-eabi/SDL/bin/sdl-config --libs`  -L$(QTDIR)/lib -L$(EZXDIR)/lib -lm -lqte-mt -lezxpm -ljpeg -lezxappbase -lezxtapi-xscale-r -llog_util -llighting #-lstdc++ 
DEFS=-DCDOGS_DATA_DIR=\"$(DATADIR)\"

# Windows data dir (current dir)
#DEFS=-DCDOGS_DATA_DIR=\"./\"

### Don't Touch These ###

dogs_OBJS = cdogs.o draw.o pics.o actors.o map.o sounds.o defs.o objs.o \
	    gamedata.o ai.o triggers.o input.o prep.o hiscores.o automap.o \
	    mission.o game.o mainmenu.o password.o files.o menu.o joystick.o \
	    sprcomp.o grafx.o blit.o text.o keyboard.o

dogsed_OBJS = cdogsed.o draw.o pics.o actors.o map.o sounds.o defs.o objs.o gamedata.o \
	      triggers.o input.o hiscores.o automap.o mission.o game.o ai.o charsed.o \
	      events.o joystick.o sprcomp.o grafx.o blit.o text.o keyboard.o name.o

### Targets ###

cdogs: $(dogs_OBJS)
	$(CC) $(dogs_OBJS) -o cdogs $(LDFLAGS)
	$(STRIP) cdogs

# editor: $(dogsed_OBJS)
# 	gcc $(dogsed_OBJS) -o dogsed $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) \
	$(DEFS) -c $<

install:
	export DESTDIR=$(DESTDIR) ; \
	export DATADIR=$(DATADIR) ; \
	export BINDIR=$(BINDIR) ; \
	./install.sh

clean:
	rm *.o cdogs

tidy:
	indent -kr -i8 *.c *.h
	rm *~

