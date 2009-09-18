TOOLPREFIX:=	/arm-eabi
#Remember to set your own TOOLPREFIX. A to pi3dec bydet!
QTDIR	:=	$(TOOLPREFIX)/lib/qt-2.3.8
EZXDIR	:=	$(TOOLPREFIX)/lib/ezx-z6
UPLOAD_PATH :=  /mmc/mmca1/bin/
CC	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-gcc
CXX	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-g++
LD	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-g++
CFLAGS	=	-pipe -Wall -W -O2 -DNO_DEBUG $(INCPATH)
CXXFLAGS=	-pipe -DQWS -fno-exceptions -fno-rtti -Wall -W -O2 -DNO_DEBUG $(INCPATH)
INCPATH	=	-I$(QTDIR)/include -I$(EZXDIR)/include -I $(TOOLPREFIX)/arm-linux-gnueabi/include
LDFLAGS	=	-s
LINK	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-gcc
#LINK	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-ld
LFLAGS	=	
LIBS	=	$(SUBLIBS) -L$(TOOLPREFIX)/arm-linux-gnueabi/lib -L$(QTDIR)/lib -L$(EZXDIR)/lib -lm -lqte-mt -lezxpm -ljpeg -lezxappbase -lezxtapi-xscale-r -llog_util -llighting -lezxam -lezxphone -lezxappsdk -lezxmsgctrviewutility -lezxmessagingsdk -lezxdb2 -ldmnative -lezxdrmsp -lezxdrmclibhack -lezxdrmsecuretime -lezxdrmcipher -lhackfs 

MOC	=	$(QTDIR)/bin/moc
UIC	=	$(QTDIR)/bin/uic

####### Files
HEADERS =   ScreenX.h\
            GUI_Define.h\
            BaseDlg.h\
            ZFileDlg.h\
            exec.h\
			ZMsg.h
            
SOURCES =   main.cpp\
            ScreenX.cpp\
            BaseDlg.cpp\
            ZFileDlg.cpp\
            exec.cpp\
			ZMsg.cpp
            
OBJECTS =   main.o ScreenX.o\
            BaseDlg.o\
            ZFileDlg.o\
            exec.o ZMsg.o
            
SRCMOC	=	moc_ScreenX.cpp\
          moc_BaseDlg.cpp\
          moc_ZFileDlg.cpp\
          moc_exec.cpp\
          moc_ZMsg.cpp
                
OBJMOC	=	moc_ScreenX.o\
          moc_BaseDlg.o\
          moc_ZFileDlg.o\
          moc_exec.o\
          moc_ZMsg.o
                
TARGETS = 	$(TARGET)
TARGET	=       AM
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
ScreenX.o: ScreenX.cpp ScreenX.h
BaseDlg.o: BaseDlg.cpp BaseDlg.h
exec: exec.cpp exec.h
ZMsg: ZMsg.cpp ZMsg.h
ZFileDlg.o: ZFileDlg.h ZFileDlg.cpp moc_ZFileDlg.cpp

#--------- moc's -----------------------------
moc_ScreenX.o: moc_ScreenX.cpp ScreenX.h
moc_ScreenX.cpp: ScreenX.h
	$(MOC) ScreenX.h -o moc_ScreenX.cpp
  
moc_BaseDlg.o: moc_BaseDlg.cpp BaseDlg.h
moc_BaseDlg.cpp: BaseDlg.h
	$(MOC) BaseDlg.h -o moc_BaseDlg.cpp

moc_exec.o: moc_exec.cpp exec.h
moc_exec.cpp: exec.h
	$(MOC) exec.h -o moc_exec.cpp	
		
moc_ZMsg.o: moc_ZMsg.cpp ZMsg.h
moc_ZMsg.cpp: ZMsg.h
	$(MOC) ZMsg.h -o moc_ZMsg.cpp		

moc_ZFileDlg.cpp: ZFileDlg.h
	$(MOC) $< -o $@
			
#----------------------------------------------
install: $(TARGETS)
	ncftpput -u root -p "" 192.168.16.2 $(UPLOAD_PATH) $(TARGETS)
