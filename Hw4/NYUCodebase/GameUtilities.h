#ifndef GameUtilities_h
#define GameUtilities_h

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>
#include <vector>
#include <string>
#include <stdio.h>

class Matrix;
class ShaderProgram;

class GameUtilities {
public:
	ShaderProgram* shader;
	const Uint8* keys;
	SDL_Event* event;
	SDL_Window* displayWindow;
	std::vector<GLuint> spriteSheets;
	bool* done;
};

float lerp(float v0, float v1, float t);

void DrawText(ShaderProgram *program, Matrix modelMatrix, int fontTexture, std::string text, float size, float spacing, float x, float y);

GLuint LoadTexture(const char *filePath, GLuint filter);

#endif // !GameUtilities_h
