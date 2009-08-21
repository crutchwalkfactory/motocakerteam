#include "Defs.h"

void Defs::SDL()
{

  SDL_Init(SDL_INIT_VIDEO);
  screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, 0, 0);

  //Cargar Fondo
  temp  = IMG_Load("image.png");
  grass = SDL_DisplayFormat(temp);
  SDL_FreeSurface(temp);

  //Cargar Mouse
  CargarMouse();

  //MenuLoop
  loop();

  /* clean up */
  SDL_FreeSurface(sprite);
  SDL_FreeSurface(grass);
  SDL_Quit();

}
