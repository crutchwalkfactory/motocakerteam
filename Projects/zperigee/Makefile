CFLAGS = -O2 `sdl-config --cflags`
LFLAGS = `sdl-config --libs` -lSDL_image
CC = arm-linux-g++
EXE = slideshow
OBJ = file_list.o fontdata.o main.o SDLFont.o slideshow.o

$(EXE):		$(OBJ)
		$(CC) $(LFLAGS) $(OBJ) -o $(EXE)

file_list.o:	file_list.cpp file_list.h
		$(CC) $(CFLAGS) -c file_list.cpp

fontdata.o:	fontdata.cpp fontdata.h
		$(CC) $(CFLAGS) -c fontdata.cpp

main.o:		main.cpp file_list.h slideshow.h cache.h
		$(CC) $(CFLAGS) -c main.cpp

SDLFont.o:	SDLFont.cpp SDLFont.h
		$(CC) $(CFLAGS) -c SDLFont.cpp

slideshow.o:	slideshow.cpp slideshow.h cache.h SDLFont.h fontdata.cpp fontdata.h file_list.h
		$(CC) $(CFLAGS) -c slideshow.cpp

clean:
		rm -f *.o $(EXE)



