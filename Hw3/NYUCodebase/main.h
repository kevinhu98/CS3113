#ifndef main_h
#define main_h

#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include "ShaderProgram.h"

enum GameMode { STATE_TITLE_SCREEN, STATE_GAME_LEVEL, STATE_GAME_OVER };

extern SDL_Event event;
extern bool done;
extern const Uint8* keys;
extern ShaderProgram program;
extern Matrix modelMatrix;

#endif // !main_h
