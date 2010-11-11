TOOLPREFIX:=	/arm-eabi-new
#Remember to set your own TOOLPREFIX. A to pi3dec bydet!
QTDIR	:=	$(TOOLPREFIX)/lib/qt-2.3.8
EZXDIR	:=	$(TOOLPREFIX)/lib/ezx-z6
UPLOAD_PATH :=  /mmc/mmca1/bin/
CC	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-gcc
CXX	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-g++
LD	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-g++
CFLAGS	=	-pipe -Wall -W -DNO_DEBUG $(INCPATH)
CXXFLAGS=	-pipe -DQWS -fno-exceptions -fno-rtti -Wall -W -DNO_DEBUG $(INCPATH)
INCPATH	=	-I$(QTDIR)/include -I$(EZXDIR)/include -I $(TOOLPREFIX)/arm-linux-gnueabi/include
LDFLAGS	=	-s
LINK	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-gcc
LFLAGS	=	
LIBS	=	$(SUBLIBS) -L$(TOOLPREFIX)/arm-linux-gnueabi/lib -L$(QTDIR)/lib -L$(EZXDIR)/lib -lm -lqte-mt -lezxpm -ljpeg -lezxappbase -lezxtapi-xscale-r -llog_util -llighting
MOC	=	$(QTDIR)/bin/moc
UIC	=	$(QTDIR)/bin/uic


####### Files
HEADERS =   texteditor.h\
            BaseDlg.h\
            ZFileDlg.h\
            ZFileBrowser.h\
            ZMultiLineEditEx.h\
            ZAboutDlg.h
            
SOURCES =   main.cpp\
            texteditor.cpp\
            BaseDlg.cpp\
            ZFileDlg.cpp\
            ZFileBrowser.cpp\
            ZMultiLineEditEx.cpp\
            ZAboutDlg.cpp
	    
OBJECTS =   main.o\
            texteditor.o\
            BaseDlg.o\
            ZFileDlg.o\
            ZFileBrowser.o\
            ZMultiLineEditEx.o\
            ZAboutDlg.o
            
INTERFACES =
UICDECLS =	
UICIMPLS =	
SRCMOC	=	moc_texteditor.cpp\
          moc_BaseDlg.cpp\
          moc_ZFileDlg.cpp\
          moc_ZFileBrowser.cpp\
          moc_ZMultiLineEditEx.cpp\
          moc_ZAboutDlg.cpp
                
OBJMOC	=	moc_texteditor.o\
          moc_BaseDlg.o\
          moc_ZFileDlg.o\
          moc_ZFileBrowser.o\
          moc_ZMultiLineEditEx.o\
          moc_ZAboutDlg.o
                
ICON_FILES = img/*
MAINFILES = zTextViewer.png settings.cfg
                
DIST	=	
TARGETS = 	$(TARGET)
TARGET	=       zTextViewer
INTERFACE_DECL_PATH = .

#AddFILES  = settings.cfg exec.png file.png folder.png link.png writable.png 

all: $(TARGET)

$(TARGET): $(UICDECLS) $(OBJECTS) $(OBJMOC) 
	$(LINK) $(LFLAGS) -o $(TARGET) $(OBJECTS) $(OBJMOC) $(LIBS)

moc: $(SRCMOC)

clean:
	-rm -f $(OBJECTS) $(OBJMOC) $(SRCMOC) $(UICIMPLS) $(UICDECLS) $(TARGET)
	-rm -f *~ core
	$(RM) *.o
	$(RM) moc_*.cpp

#------- obj ------------------------------------------------------
main.o: main.cpp
texteditor.o: texteditor.h texteditor.cpp moc_texteditor.cpp
BaseDlg.o: BaseDlg.h BaseDlg.cpp moc_BaseDlg.cpp
ZFileDlg.o: ZFileDlg.h ZFileDlg.cpp moc_ZFileDlg.cpp
ZFileBrowser.o: ZFileBrowser.h ZFileBrowser.cpp moc_ZFileBrowser.cpp
ZMultiLineEditEx.o: ZMultiLineEditEx.h ZMultiLineEditEx.cpp moc_ZMultiLineEditEx.cpp
ZAboutDlg.o: ZAboutDlg.h ZAboutDlg.cpp moc_ZAboutDlg.cpp
#------  moc -------------------------------------------------------
moc_texteditor.cpp: texteditor.h
	$(MOC) $< -o $@

moc_BaseDlg.cpp: BaseDlg.h
	$(MOC) $< -o $@
	
moc_ZFileDlg.cpp: ZFileDlg.h
	$(MOC) $< -o $@
	
moc_ZFileBrowser.cpp: ZFileBrowser.h
	$(MOC) $< -o $@
	
moc_ZMultiLineEditEx.cpp: ZMultiLineEditEx.h	
	$(MOC) $< -o $@
moc_ZAboutDlg.cpp: ZAboutDlg.h	
	$(MOC) $< -o $@
#-------------------------------------------------------------------
install: $(TARGETS)
	ncftpput -u root -p "" 192.168.16.2 $(UPLOAD_PATH) $(TARGETS)
	ncftpput -u root -p "" 192.168.16.2 $(UPLOAD_PATH) $(MAINFILES)
	ncftpput -u root -p "" 192.168.16.2 $(UPLOAD_PATH)/img $(ICON_FILES) 	 
