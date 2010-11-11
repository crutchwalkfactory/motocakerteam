TOOLPREFIX:=	/arm-eabi
#Remember to set your own TOOLPREFIX. A to pi3dec bydet!
QTDIR	:=	$(TOOLPREFIX)/lib/qt-2.3.8
EZXDIR	:=	$(TOOLPREFIX)/lib/ezx-z6
MOTODIR	:=	/home/matias/Motorola/MOTODEV_Studio_for_Linux_v0.3.0/MOTOMAGX_0.3_SDK
UPLOAD_PATH :=  /mmc/mmca1/bin/
CC	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-gcc
CXX	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-g++
LD	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-g++
CFLAGS	=	-pipe -Wall -W -DNO_DEBUG $(INCPATH)
CXXFLAGS=	-pipe -DQWS -fno-exceptions -fno-rtti -Wall -W -DNO_DEBUG $(INCPATH)
INCPATH	=	-I$(QTDIR)/include -I$(EZXDIR)/include -I $(TOOLPREFIX)/arm-linux-gnueabi/include -I$(MOTODIR)/include -I$(MOTODIR)/include/linux/x86 -I$(MOTODIR)/private
LDFLAGS	=	-s
LINK	=	$(TOOLPREFIX)/bin/arm-linux-gnueabi-gcc
LFLAGS	=	
LIBS	=	$(SUBLIBS) -L$(TOOLPREFIX)/arm-linux-gnueabi/lib -L$(QTDIR)/lib -L$(EZXDIR)/lib -lm -lqte-mt -lezxpm -ljpeg -lezxappbase -lezxtapi-xscale-r -llog_util -llighting -lezxthemesetup -lezxsecuritysetup -lezxdrmsp -lezxmystuff-base -lezxjpegutils -lezxam -lezxdrmcipher -lezxdrmsecuretime -lezxsort -ljanus -lezxexif -ltpa -lsahara -lmotsecurity -lezxam
MOC	=	$(QTDIR)/bin/moc
UIC	=	$(QTDIR)/bin/uic


####### Files
HEADERS =   launcher.h\
            BaseDlg.h\
            ZFileDlg.h\
            ZFileBrowser.h\
            ZAboutDlg.h\
            myPopup.h\
            
SOURCES =   main.cpp\
            launcher.cpp\
            BaseDlg.cpp\
            ZFileDlg.cpp\
            ZFileBrowser.cpp\
            ZAboutDlg.cpp\
            myPopup.cpp\
	    
OBJECTS =   main.o\
            launcher.o\
            BaseDlg.o\
            ZFileDlg.o\
            ZFileBrowser.o\
            ZAboutDlg.o\
            myPopup.o\
            
INTERFACES =
UICDECLS =	
UICIMPLS =	
SRCMOC	=	moc_launcher.cpp\
          moc_BaseDlg.cpp\
          moc_ZFileDlg.cpp\
          moc_ZFileBrowser.cpp\
          moc_ZAboutDlg.cpp\
          moc_myPopup.cpp\
                
OBJMOC	=	moc_launcher.o\
          moc_BaseDlg.o\
          moc_ZFileDlg.o\
          moc_ZFileBrowser.o\
          moc_ZAboutDlg.o\
          moc_myPopup.o\
                
ICON_FILES = img/*
MAINFILES = zLauncher.png
#settings.cfg
                
DIST	=	
TARGETS = 	$(TARGET)
TARGET	=       zFileManager
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
launcher.o: launcher.h launcher.cpp moc_launcher.cpp
BaseDlg.o: BaseDlg.h BaseDlg.cpp moc_BaseDlg.cpp
ZFileDlg.o: ZFileDlg.h ZFileDlg.cpp moc_ZFileDlg.cpp
ZFileBrowser.o: ZFileBrowser.h ZFileBrowser.cpp moc_ZFileBrowser.cpp
ZAboutDlg.o: ZAboutDlg.h ZAboutDlg.cpp moc_ZAboutDlg.cpp
myPopup.o: myPopup.h myPopup.cpp moc_myPopup.cpp
#------  moc -------------------------------------------------------
moc_launcher.cpp: launcher.h
	$(MOC) $< -o $@

moc_BaseDlg.cpp: BaseDlg.h
	$(MOC) $< -o $@
	
moc_ZFileDlg.cpp: ZFileDlg.h
	$(MOC) $< -o $@
	
moc_ZFileBrowser.cpp: ZFileBrowser.h
	$(MOC) $< -o $@

moc_ZAboutDlg.cpp: ZAboutDlg.h	
	$(MOC) $< -o $@

moc_myPopup.cpp: myPopup.h	
	$(MOC) $< -o $@
#-------------------------------------------------------------------
install: $(TARGETS)
	ncftpput -u root -p "" 192.168.16.2 $(UPLOAD_PATH) $(TARGETS)
	ncftpput -u root -p "" 192.168.16.2 $(UPLOAD_PATH) $(MAINFILES)
	ncftpput -u root -p "" 192.168.16.2 $(UPLOAD_PATH)/img $(ICON_FILES) 	 
