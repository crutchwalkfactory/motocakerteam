#include "Defs.h"

void Defs::CargarMouse()
{

  /* set sprite position */
  rcSprite.x = 0;
  rcSprite.y = 0;

  /* load sprite */
  temp   = IMG_Load("puntero.png");
  sprite = SDL_DisplayFormatAlpha(temp);
  SDL_FreeSurface(temp);

  /* setup sprite colorkey and turn on RLE */
  colorkey = SDL_MapRGB(screen->format, 255, 0, 255);
  SDL_SetColorKey(sprite, SDL_SRCCOLORKEY | SDL_RLEACCEL, colorkey);

}

void Defs::IniciarMouse()
{

  SDL_BlitSurface(sprite, NULL, screen, &rcSprite);

}
