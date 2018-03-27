#include <vector>
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "main.h"
#include "SheetSprite.h"
#include "Entity.h"
#include "GameState.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6

/*To-do List Priority: 
1. Enemys currently do not disappear
2. Enemys need to shoot back 
3. Shoot Timer so that player cannot hold space
4. Possible increasing difficulty
*/

SDL_Window* displayWindow;

void DrawText(ShaderProgram *program, Matrix modelMatrix, int fontTexture, std::string text, float size, float spacing, float x, float y) {
	modelMatrix.Identity();
	modelMatrix.Translate(x, y, 0);
	program->SetModelMatrix(modelMatrix);

	float texture_size = 1.0 / 16.0f;
	std::vector<float> vertexData;
	std::vector<float> texCoordData;

	for (int i = 0; i < text.size(); i++) {
		int spriteIndex = (int)text[i];
		float texture_x = (float)(spriteIndex % 16) / 16.0f;
		float texture_y = (float)(spriteIndex / 16) / 16.0f;

		vertexData.insert(vertexData.end(), {
			((size + spacing) * i) + (-0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (0.5f * size), -0.5f * size, 
			((size + spacing) * i) + (0.5f * size), 0.5f * size,
			((size + spacing) * i) + (-0.5f * size), -0.5f * size,
			});
		texCoordData.insert(texCoordData.end(), {
			texture_x, texture_y,
			texture_x, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x + texture_size, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x, texture_y + texture_size,
			});
	}
	glBindTexture(GL_TEXTURE_2D, fontTexture);

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program->positionAttribute);

	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program->texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, vertexData.size()/2);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

GLuint LoadTexture(const char *filePath) {
	int w, h, comp;
	unsigned char* image = stbi_load(filePath, &w, &h, &comp, STBI_rgb_alpha);

	if (image == NULL) {
		std::cout << "Unable to load image. Make sure the path is correct \n";
		assert(false);
	}

	GLuint retTexture;
	glGenTextures(1, &retTexture);
	glBindTexture(GL_TEXTURE_2D, retTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(image);
	return retTexture;
}

ShaderProgram program;
Matrix projectionMatrix;
Matrix modelMatrix;
Matrix viewMatrix;
GLuint font;
GLuint spriteSheetTexture;
const Uint8 *keys = SDL_GetKeyboardState(NULL);
SDL_Event event;
GameMode mode;
GameState gameState;
bool done = false;

SheetSprite Enemy = SheetSprite(spriteSheetTexture, 423.0f / 1024, 728.0f / 1024, 93.0f / 1024, 84.0f / 1024, 0.2);
SheetSprite Player1 = SheetSprite(spriteSheetTexture, 211.0f / 1024, 941.0f / 1024, 93.0f / 1024, 84.0f / 1024, 0.2);
SheetSprite Bullet = SheetSprite(spriteSheetTexture, 858.0f / 1024, 230.0f / 1024, 9.0f / 1024, 54.0 / 1024, 0.2);

void TitleScreenInput() {
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			done = true;
		}
		else if (event.type == SDL_KEYDOWN) {
			if (event.key.keysym.scancode == SDL_SCANCODE_SPACE)  {
				if (mode == STATE_GAME_OVER) {
					gameState.Initialize(&Player1, &Bullet, &Enemy);
				}
				mode = STATE_GAME_LEVEL;
			}
		}
	}
}

void DrawTitleScreen() {
	DrawText(&program, modelMatrix, font, "Space Invaders", 0.25, 0, -1.5, 1);
	DrawText(&program, modelMatrix, font, "Press Space", 0.15, 0, -0.75, -1);
}

void GameOverScreen() {
	DrawText(&program, modelMatrix, font, "Game Over", 0.35, 0, -1.5, 1);
	DrawText(&program, modelMatrix, font, "Press Space to Replay", 0.15, 0, -1.65, -1);
}

void Render() {
	
	switch (mode) {
	case STATE_TITLE_SCREEN:
		DrawTitleScreen();
		break;
	case STATE_GAME_LEVEL:
		gameState.Render();
		break;
	case STATE_GAME_OVER:
		GameOverScreen();
		break;
	}
}
void Update(float elapsed) {
	switch (mode) {
	case STATE_TITLE_SCREEN:
		break;
	case STATE_GAME_LEVEL:
		gameState.Update(elapsed);
		break;
	case STATE_GAME_OVER:
		break;
	}
}
void ProcessInput() {
	switch (mode) {
	case STATE_TITLE_SCREEN:
		TitleScreenInput();
	case STATE_GAME_OVER:
		TitleScreenInput();
		break;
	case STATE_GAME_LEVEL:
		gameState.ProcessInput();
		break;
	}
}

int main(int argc, char *argv[])
{
	float width = 1280;
	float length = 720;

	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Space Invaders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, length, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif
	glViewport(0, 0, width, length);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	font = LoadTexture(RESOURCE_FOLDER"font.png");
	spriteSheetTexture = LoadTexture(RESOURCE_FOLDER"ships.png");

	SheetSprite Enemy = SheetSprite(spriteSheetTexture, 423.0f / 1024, 728.0f / 1024, 93.0f / 1024, 84.0f / 1024, 0.2);
	SheetSprite Player1 = SheetSprite(spriteSheetTexture, 211.0f / 1024, 941.0f / 1024, 93.0f / 1024, 84.0f / 1024, 0.2);
	SheetSprite Bullet = SheetSprite(spriteSheetTexture, 858.0f / 1024, 230.0f / 1024, 9.0f / 1024, 54.0 / 1024, 0.2);

	program.Load(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	projectionMatrix.SetOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);
	program.SetProjectionMatrix(projectionMatrix);
	program.SetViewMatrix(viewMatrix);

	gameState.Initialize(&Player1, &Bullet, &Enemy);

	float lastFrameTicks = 0.0f;
	float accumulator = 0.0f;

	//gameState.player->render(program, modelMatrix);
	while (!done) {
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
		
		ProcessInput();
	
		elapsed += accumulator;
		if (elapsed < FIXED_TIMESTEP) {
			accumulator = elapsed;
			continue;	
		}

		while (elapsed >= FIXED_TIMESTEP) {
			Update(FIXED_TIMESTEP);
			elapsed -= FIXED_TIMESTEP;
		}
		accumulator = elapsed;
		
		glClear(GL_COLOR_BUFFER_BIT);
		
  		Render();

		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}

