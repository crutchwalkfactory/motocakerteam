TOOLPREFIX:=	/arm-eabi
#Remember to set your own TOOLPREFIX. A to pi3dec bydet!
QTDIR	:=	$(TOOLPREFIX)/lib/qt-2.3.8
EZXDIR	:=	$(TOOLPREFIX)/lib/ezx-z6
MOTODIR	:=	/home/matias/Motorola/MOTODEV_Studio_for_Linux_v0.3.0/MOTOMAGX_0.3_SDK
UPLOAD_PATH :=  /mmc/mmca1/bin/
CC	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-gcc
CXX	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-g++
LD	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-g++
CFLAGS	=	-pipe -Wall -W -O2 -DNO_DEBUG $(INCPATH)
CXXFLAGS=	-pipe -DQWS -fno-exceptions -fno-rtti -Wall -W -O2 -DNO_DEBUG $(INCPATH)
INCPATH	=	-I$(QTDIR)/include -I$(EZXDIR)/include -I $(TOOLPREFIX)/arm-linux-gnueabi/include -I$(MOTODIR)/include -I$(MOTODIR)/include/linux/x86
LDFLAGS	=	-s
LINK	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-gcc
LFLAGS	=	
LIBS	=	$(SUBLIBS) -L$(TOOLPREFIX)/arm-linux-gnueabi/lib -L$(QTDIR)/lib -L$(EZXDIR)/lib -lm -lqte-mt -lezxpm -ljpeg -lezxappbase -lezxtapi-xscale-r -llog_util -llighting
MOC	=	$(QTDIR)/bin/moc
UIC	=	$(QTDIR)/bin/uic


####### Files
HEADERS =   vr.h\
            
SOURCES =   main.cpp\
            vr.cpp\
	    
OBJECTS =   main.o vr.o\
            
SRCMOC	= moc_vr.cpp\
               
OBJMOC	= moc_vr.o\
                
TARGETS = 	$(TARGET)
TARGET	=       vr
INTERFACE_DECL_PATH = .



all: $(TARGET)

$(TARGET): $(UICDECLS) $(OBJECTS) $(OBJMOC) 
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJMOC) $(LIBS)

moc: $(SRCMOC)

clean:
	-rm -f $(OBJECTS) $(OBJMOC) $(SRCMOC) $(UICIMPLS) $(UICDECLS) $(TARGET)
	-rm -f *~ core
	$(RM) *.o
	$(RM) moc_*.cpp

#----------- obj -----------------------------
main.o: main.cpp
vr.o: vr.cpp vr.h

#--------- moc's -----------------------------
moc_vr.o: moc_vr.cpp vr.h
moc_vr.cpp: vr.h
	$(MOC) vr.h -o moc_vr.cpp
  
#----------------------------------------------
install: $(TARGETS)
	ncftpput -u root -p "" 192.168.16.2 $(UPLOAD_PATH) $(TARGETS)
