SUBDIRS = capture Documentation filter sources

dist_pkgdata_DATA = \
	LucidaSansRegular.ttf LucidaTypewriterRegular.ttf \
	SDLcam.cfg SDLcam.xml

bin_PROGRAMS = SDLcam

SDLcam_SOURCES = \
	main.cpp init.c SDL_v4l.cpp SDL_v4l_filters.c webcam.cpp interface.cpp \
	configuration.cpp config_file.c SDL_v4l_filters_thomas.c dump_thomas.c \
	capture.c ftp-upload.c logo.c parser.c misc.c xml_theme.cpp dyn_misc.c \
	*.h
SDLcam_CPPFLAGS = $(SDL_CFLAGS) $(XML2_CFLAGS)
SDLcam_LDADD    = $(SDL_LIBS) $(XML2_LIBS) $(JPEG_LIBS) $(PNG_LIBS)
SDLcam_LDFLAGS  = -export-dynamic

regen_am:
	for d in $(SUBDIRS) ; do \
		( \
		cd $$d ; \
		$(MAKE) -s -f ../gen.mk > Makefile.am ; \
		) || exit $$? ; \
	done
