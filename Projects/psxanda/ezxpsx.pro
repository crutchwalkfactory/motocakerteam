TEMPLATE	= app
CONFIG	   += qt warn_on release

TMAKE_CFLAGS	+= -fsigned-char -ffast-math
TMAKE_CXXFLAGS	+= -fsigned-char -ffast-math

HEADERS		= main.h configfile.h gamewidget.h ezxaudio.h font.h psx_player.h\
			PsxCommon.h plugins.h \
			R3000A.h PsxMem.h PsxHw.h PsxBios.h PsxDma.h PsxCounters.h PsxHLE.h \
			Sio.h Spu.h Mdec.h Gte.h Misc.h CdRom.h System.h \
			GPU_externals.h prim.h soft.h

SOURCES		= main.cpp gamewidget.cpp ezxaudio.cpp font.cpp psx_player.cpp \
			R3000A.c PsxMem.c PsxHw.c PsxBios.c PsxDma.c PsxCounters.c PsxHLE.c \
			Sio.c Spu.c Mdec.c Gte.c Misc.c CdRom.c PsxInterpreter.c BCDCoder.c \
			plugins_CDR.cpp plugins_SPU.cpp plugins_PAD.cpp plugins_GPU.cpp plugins_NET.cpp \
			plugin.cpp \
			prim.cpp soft.cpp draw.cpp

#			DisR3000A.c

TARGET		= ezxpsx/ezxpsx

INCLUDEPATH+= $(EZX_BASE)/include/3pt
DEFINES	   += __LINUX__

LIBS	   += -lz

MOC_DIR     = .moc
OBJECTS_DIR = .obj
