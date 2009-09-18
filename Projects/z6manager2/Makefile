TOOLPREFIX:=	/arm-eabi
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
LIBS	=	$(SUBLIBS) -L$(TOOLPREFIX)/arm-linux-gnueabi/lib -L$(QTDIR)/lib -L$(EZXDIR)/lib -lm -lqte-mt -lezxpm -ljpeg -lezxappbase -lezxtapi-xscale-r -llog_util -llighting
MOC	=	$(QTDIR)/bin/moc
UIC	=	$(QTDIR)/bin/uic


####### Files
HEADERS =   z6manager.h\
            GUI_Define.h\
            BaseDlg.h\
            ZAboutDlg.h\
            ZFileDlg.h\
            ZFileBrowser.h\

            
SOURCES =   main.cpp\
            z6manager.cpp\
            BaseDlg.cpp\
            ZAboutDlg.cpp\
            ZFileDlg.cpp\
            ZFileBrowser.cpp\
	    
OBJECTS =   main.o z6manager.o\
            BaseDlg.o\
            ZAboutDlg.o\
            ZFileDlg.o\
            ZFileBrowser.o\
            
SRCMOC	= moc_z6manager.cpp\
          moc_BaseDlg.cpp\
          moc_ZAboutDlg.cpp\
          moc_ZFileDlg.cpp\
          moc_ZFileBrowser.cpp\
               
OBJMOC	= moc_z6manager.o\
          moc_BaseDlg.o\
          moc_ZAboutDlg.o\
          moc_ZFileDlg.o\
          moc_ZFileBrowser.o\
                
TARGETS = 	$(TARGET)
TARGET	=       z6manager
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
z6manager.o: z6manager.cpp z6manager.h
BaseDlg.o: BaseDlg.cpp BaseDlg.h
ZAboutDlg.o: ZAboutDlg.cpp ZAboutDlg.h
ZFileDlg.o: ZFileDlg.h ZFileDlg.cpp moc_ZFileDlg.cpp
ZFileBrowser.o: ZFileBrowser.h ZFileBrowser.cpp moc_ZFileBrowser.cpp

#--------- moc's -----------------------------
moc_z6manager.o: moc_z6manager.cpp z6manager.h
moc_z6manager.cpp: z6manager.h
	$(MOC) z6manager.h -o moc_z6manager.cpp
  
moc_BaseDlg.o: moc_BaseDlg.cpp BaseDlg.h
moc_BaseDlg.cpp: BaseDlg.h
	$(MOC) BaseDlg.h -o moc_BaseDlg.cpp

moc_ZAboutDlg.o: moc_ZAboutDlg.cpp ZAboutDlg.h
moc_ZAboutDlg.cpp: ZAboutDlg.h
	$(MOC) ZAboutDlg.h -o moc_ZAboutDlg.cpp

moc_ZFileDlg.cpp: ZFileDlg.h
	$(MOC) $< -o $@
	
moc_ZFileBrowser.cpp: ZFileBrowser.h
	$(MOC) $< -o $@
	
#----------------------------------------------
install: $(TARGETS)
	ncftpput -u root -p "" 192.168.16.2 $(UPLOAD_PATH) $(TARGETS)
