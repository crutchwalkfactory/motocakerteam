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
LFLAGS	=	
LIBS	=	$(SUBLIBS) -L$(TOOLPREFIX)/arm-linux-gnueabi/lib -L$(QTDIR)/lib -L$(EZXDIR)/lib -lm -lqte-mt -lezxpm -ljpeg -lezxappbase -lezxtapi-xscale-r -llog_util -llighting -lezxam -lezxphone -lezxappsdk -lezxmsgctrviewutility -lezxmessagingsdk -lezxdb2 -ldmnative -lezxdrmsp -lezxdrmclibhack -lezxdrmsecuretime -lezxdrmcipher -lhackfs -lzcopchannel -lezxalert -lezxidle -lezxalarmagent -lezxalarmclock -lezxquickalarmapi -lezxalarmserver -lezxbluetooth -lezxmbtapi

MOC	=	$(QTDIR)/bin/moc
UIC	=	$(QTDIR)/bin/uic


####### Files
HEADERS =   zgui.h\
            GUI_Define.h\
            BaseDlg.h\
            ZFileDlg.h\
            myPopup.h\
            
SOURCES =   main.cpp\
            zgui.cpp\
            BaseDlg.cpp\
            ZFileDlg.cpp\
            myPopup.cpp\
	    
OBJECTS =   main.o zgui.o\
            BaseDlg.o\
            ZFileDlg.o\
            myPopup.o\
            
SRCMOC	=	moc_zgui.cpp\
          moc_BaseDlg.cpp\
          moc_ZFileDlg.cpp\
          moc_myPopup.cpp\
                
OBJMOC	=	moc_zgui.o\
          moc_BaseDlg.o\
          moc_ZFileDlg.o\
          moc_myPopup.o\
                
TARGETS = 	$(TARGET)
TARGET	=       zNotepad
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
zgui.o: zgui.cpp zgui.h
BaseDlg.o: BaseDlg.h BaseDlg.cpp moc_BaseDlg.cpp
ZFileDlg.o: ZFileDlg.h ZFileDlg.cpp moc_ZFileDlg.cpp
myPopup.o: myPopup.h myPopup.cpp moc_myPopup.cpp
#--------- moc's -----------------------------
moc_zgui.o: moc_zgui.cpp zgui.h
moc_zgui.cpp: zgui.h
	$(MOC) zgui.h -o moc_zgui.cpp
moc_BaseDlg.cpp: BaseDlg.h
	$(MOC) $< -o $@
	
moc_ZFileDlg.cpp: ZFileDlg.h
	$(MOC) $< -o $@
	
moc_myPopup.cpp: myPopup.h	
	$(MOC) $< -o $@
#----------------------------------------------
install: $(TARGETS)
	ncftpput -u root -p "" 192.168.16.2 $(UPLOAD_PATH) $(TARGETS)
