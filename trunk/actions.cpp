#include "Defs.h"

void Defs::loop()
{
  gameover = 0;

  while (!gameover)
  {
    if (SDL_PollEvent(&event))
      {
      switch (event.type) 
	  {
        case SDL_KEYDOWN:
          switch (event.key.keysym.sym) 
	    {
            //case SDLK_ESCAPE: gameover = 1; break;
	    case SDLK_RETURN: Acciones(); break;
	    }
          }
      }

  keystate = SDL_GetKeyState(NULL);
  if (keystate[SDLK_LEFT] ) { rcSprite.x -= 3; }
  if (keystate[SDLK_RIGHT] ) { rcSprite.x += 3; }
  if (keystate[SDLK_UP] ) { rcSprite.y -= 3; }
  if (keystate[SDLK_DOWN] ) { rcSprite.y += 3; }

  //Para que no se vaya de los bordes

  if ( rcSprite.x < 0 ) { rcSprite.x = 0; }
  else if ( rcSprite.x > SCREEN_WIDTH-SPRITE_SIZE ) { rcSprite.x = SCREEN_WIDTH-SPRITE_SIZE; }
  if ( rcSprite.y < 0 ) { rcSprite.y = 0; }
  else if ( rcSprite.y > SCREEN_HEIGHT-SPRITE_SIZE ) { rcSprite.y = SCREEN_HEIGHT-SPRITE_SIZE; }

  //Debug
  //system(QString("echo X = %1                Y = %2").arg(rcSprite.x).arg(rcSprite.y));

  //Dibujar Fondo
  SDL_BlitSurface(grass, NULL, screen, &rcGrass);

  //Iniciar Mouse
  IniciarMouse();

  //Refrescar Pantalla
  SDL_UpdateRect(screen, 0, 0, 0, 0);
  SDL_Delay(10);
  }
}

void Defs::Acciones()
{

  //Jugar Ahora
  if ( rcSprite.x >= 104 && rcSprite.x <= 196 && rcSprite.y >= 130 && rcSprite.y <= 144 ) 
  { system("echo \"Jugar Ahora\""); }

  //Sistema Ubeda
  if ( rcSprite.x >= 84 && rcSprite.x <= 218 && rcSprite.y >= 148 && rcSprite.y <= 160 ) 
  { system("echo \"Sistema Ubeda\""); }

  //Ayuda
  if ( rcSprite.x >= 130 && rcSprite.x <= 172 && rcSprite.y >= 166 && rcSprite.y <= 176 ) 
  { system("echo \"Ayuda\""); }

  //Creditos
  if ( rcSprite.x >= 120 && rcSprite.x <= 182 && rcSprite.y >= 182 && rcSprite.y <= 192 ) 
  { system("echo \"Creditos\""); }

  //Salir
  if ( rcSprite.x >= 132 && rcSprite.x <= 172 && rcSprite.y >= 190 && rcSprite.y <= 208 ) 
  { gameover = 1; }

}
