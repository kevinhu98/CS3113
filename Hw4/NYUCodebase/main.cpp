#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include "SheetSprite.h"
#include "Entity.h"
#include "GameState.h"
#include "FlareMap.h"
#include "GameUtilities.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6


SDL_Window* displayWindow;

ShaderProgram program;
Matrix projectionMatrix;
Matrix modelMatrix;
Matrix viewMatrix;
GLuint spriteSheetTexture;
const Uint8 *keys = SDL_GetKeyboardState(NULL);
SDL_Event event;
GameState gameState;
bool done = false;
FlareMap map;
GameUtilities Utilities;

int main(int argc, char *argv[])
{
	float width = 1280;
	float length = 720;
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Platformer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, length, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif
	glViewport(0, 0, width, length);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	program.Load(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	projectionMatrix.SetOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);
	program.SetProjectionMatrix(projectionMatrix);
	program.SetViewMatrix(viewMatrix);

	spriteSheetTexture = LoadTexture(RESOURCE_FOLDER"arne_sprites.png", GL_NEAREST);
	map.setSpriteSheet(spriteSheetTexture, 16, 8);
	map.Load(RESOURCE_FOLDER"hw4.txt");
	
	Utilities.event = &event;
	Utilities.keys = keys;
	
	Utilities.shader = &program;
	Utilities.displayWindow = displayWindow;
	Utilities.done = &done;
	Utilities.spriteSheets.push_back(spriteSheetTexture);
	gameState.Initialize(&Utilities, &map);
	
	float lastFrameTicks = 0.0f;
	float accumulator = 0.0f;

	while (!done) {
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
		
		gameState.ProcessInput();
	
		elapsed += accumulator;
		if (elapsed < FIXED_TIMESTEP) {
			accumulator = elapsed;
			continue;	
		}

		while (elapsed >= FIXED_TIMESTEP) {
			gameState.Update(FIXED_TIMESTEP);
			elapsed -= FIXED_TIMESTEP;
		}
		accumulator = elapsed;
		
		glClear(GL_COLOR_BUFFER_BIT);
		
		gameState.Render();

		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}

