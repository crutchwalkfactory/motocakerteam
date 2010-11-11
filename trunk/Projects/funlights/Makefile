TOOLPREFIX:=	/arm-eabi
#Remember to set your own TOOLPREFIX. A to pi3dec bydet!
QTDIR	:=	$(TOOLPREFIX)/lib/qt-2.3.8
EZXDIR	:=	$(TOOLPREFIX)/lib/ezx-z6
EZXDIR_U9	:=	$(TOOLPREFIX)/lib/ezx-u9
UPLOAD_PATH :=  /mmc/mmca2/
CC	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-gcc
CXX	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-g++
LD	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-g++
CFLAGS	=	-pipe -Wall -W -O2 -DNO_DEBUG $(INCPATH)
CXXFLAGS=	-pipe -DQWS -fno-exceptions -fno-rtti -Wall -W -O2 -DNO_DEBUG $(INCPATH)
INCPATH	=	-I$(QTDIR)/include -I$(EZXDIR)/include -I $(TOOLPREFIX)/arm-linux-gnueabi/include
LDFLAGS	=	-s
LINK	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-gcc
LFLAGS	=	
LIBS	=	$(SUBLIBS) -L$(TOOLPREFIX)/arm-linux-gnueabi/lib -L$(QTDIR)/lib -L$(EZXDIR)/lib -lm -lqte-mt -lezxpm -ljpeg -lezxappbase -lezxtapi-xscale-r -llog_util -llighting
MOC	=	$(QTDIR)/bin/moc
UIC	=	$(QTDIR)/bin/uic


####### Files
HEADERS =   demonio.h
            
SOURCES =   main.cpp demonio.cpp
	    
OBJECTS =   main.o demonio.o
            
INTERFACES =
UICDECLS =	
UICIMPLS =	
SRCMOC	=	moc_demonio.cpp
                
OBJMOC	=	moc_demonio.o
                
DIST	=	
TARGETS = 	$(TARGET)
TARGETS_U9 = 	$(TARGET_U9)
TARGET	=       funlights
INTERFACE_DECL_PATH = .



all: $(TARGET)

$(TARGET): $(UICDECLS) $(OBJECTS) $(OBJMOC) 
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJMOC) $(LIBS)
	/arm-eabi/bin/arm-linux-strip -s $(TARGET)
	
		
moc: $(SRCMOC)

clean:
	-rm -f $(OBJECTS) $(OBJMOC) $(SRCMOC) $(UICIMPLS) $(UICDECLS) $(TARGET) $(TARGET_U9)
	-rm -f *~ core
	$(RM) *.o
	$(RM) moc_*.cpp

#--------- moc's -----------------------------
moc_%.cpp: %.h
	$(MOC) $< -o $@

moc_ZMsg.o: moc_ZMsg.cpp ZMsg.h
moc_ZMsg.cpp: ZMsg.h
	$(MOC) ZMsg.h -o moc_ZMsg.cpp		

install: $(TARGETS)
	ncftpput -u root -p "" 192.168.16.2 $(UPLOAD_PATH) $(TARGETS)
