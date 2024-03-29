# GNU Makefile for hexen2 genmodel tool using GCC.
# $Header: /cvsroot/uhexen2/utils/genmodel/Makefile,v 1.21 2008/04/03 09:25:54 sezero Exp $
#
# if building a debug version :		make DEBUG=1 [other stuff]
#
# To cross-compile for Win32 on Unix, you must pass the WINBUILD=1
# argument to make. It would be best if you examine the script named
# build_cross_win32.sh for cross compilation.

# Path settings:
# main uhexen2 relative path
UHEXEN2_TOP:=../..
# where the common sources/objects are
COMMONDIR:=../common
# where to put the binaries
DESTDIR  :=../bin

# include the common dirty stuff
include $(UHEXEN2_TOP)/scripts/makefile.inc

# Names of the binaries
ifeq ($(TARGET_OS),win32)
BINARY:=genmodel.exe
endif
ifeq ($(TARGET_OS),unix)
BINARY:=genmodel
endif

# Compiler flags

CPUFLAGS:=
# Overrides for the default CPUFLAGS
ifeq ($(MACH_TYPE),x86)
CPUFLAGS:=-march=i586
endif

# Overrides for the default ARCHFLAGS
#ARCHFLAGS:=

ifdef DEBUG
CFLAGS := -g -Wall
else
CFLAGS := $(CPUFLAGS) -O2 -Wall -ffast-math -fexpensive-optimizations
endif

CFLAGS := $(CFLAGS) $(ARCHFLAGS)


# Other build flags

ifeq ($(TARGET_OS),win32)
INCLUDES:= -I$(W32STUFF) -I$(COMMONDIR)
LDFLAGS := -mconsole
LDLIBS  :=
endif
ifeq ($(TARGET_OS),unix)
INCLUDES:= -I$(COMMONDIR)
LDFLAGS :=
LDLIBS  := -lm
endif

# Targets
all : $(BINARY)

# Rules for turning source files into .o files
%.o: %.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<
# special rule for mathlib because of DOUBLEVEC_T
mathlib.o: $(COMMONDIR)/mathlib.c
	$(CC) -c $(CFLAGS) $(INCLUDES) -o $@ $<

# Objects
GENMODELOBJS= $(COMMONDIR)/cmdlib.o \
	$(COMMONDIR)/strlcat.o \
	$(COMMONDIR)/strlcpy.o \
	$(COMMONDIR)/q_endian.o \
	$(COMMONDIR)/byteordr.o \
	$(COMMONDIR)/util_io.o \
	$(COMMONDIR)/pathutil.o \
	$(COMMONDIR)/scriplib.o \
	$(COMMONDIR)/qdir.o \
	mathlib.o \
	loadtri.o \
	token.o \
	genmodel.o

$(BINARY) : $(GENMODELOBJS)
	$(LINKER) -o $(DESTDIR)/$(BINARY) $(GENMODELOBJS) $(LDFLAGS) $(LDLIBS)

clean : 
	rm -f *.o $(COMMONDIR)/*.o core

