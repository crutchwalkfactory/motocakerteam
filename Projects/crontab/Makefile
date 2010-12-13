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
HEADERS =   zgui.h\
            GUI_Define.h\
            ZFileDlg.h\
            BaseDlg.h\
            
SOURCES =   main.cpp\
            zgui.cpp\
            ZFileDlg.cpp\
            BaseDlg.cpp\
	    
OBJECTS =   main.o zgui.o\
            ZFileDlg.o\
            BaseDlg.o\
            
SRCMOC	= moc_zgui.cpp\
          moc_ZFileDlg.cpp\
          moc_BaseDlg.cpp\
               
OBJMOC	= moc_zgui.o\
          moc_ZFileDlg.o\
          moc_BaseDlg.o\
                
TARGETS = 	$(TARGET)
TARGET	=       crontab
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
ZFileDlg.o: ZFileDlg.h ZFileDlg.cpp moc_ZFileDlg.cpp
BaseDlg.o: BaseDlg.cpp BaseDlg.h

#--------- moc's -----------------------------
moc_zgui.o: moc_zgui.cpp zgui.h
moc_zgui.cpp: zgui.h
	$(MOC) zgui.h -o moc_zgui.cpp
moc_ZFileDlg.cpp: ZFileDlg.h
	$(MOC) $< -o $@
moc_BaseDlg.o: moc_BaseDlg.cpp BaseDlg.h
moc_BaseDlg.cpp: BaseDlg.h
	$(MOC) BaseDlg.h -o moc_BaseDlg.cpp
  
#----------------------------------------------
install: $(TARGETS)
	ncftpput -u root -p "" 192.168.16.2 $(UPLOAD_PATH) $(TARGETS)
