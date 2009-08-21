#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240
#define SPRITE_SIZE    32

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>

#include <qstring.h>

class Defs
{
  public:
    SDL_Surface *screen, *temp, *sprite, *grass;
    SDL_Rect rcSprite, rcGrass;
    SDL_Event event;
    Uint8 *keystate;
    TTF_Font * Fuente;
    int colorkey, gameover;

    void Acciones();
    void SDL();
    void CargarMouse();
    void IniciarMouse();
    void loop();

};
