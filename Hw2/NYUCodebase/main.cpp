#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "ShaderProgram.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

SDL_Window* displayWindow;

class Entity {
public:
	Entity(float width,float height, float x_pos, float y_pos, float velocity_x, float velocity_y) :
		width(width), height(height), x_pos(x_pos), y_pos(y_pos), velocity_x(velocity_x), velocity_y(velocity_y) {};
	void Draw(ShaderProgram &program) {
		float vertices[] = { x_pos - (0.5*width), y_pos - (0.5*height),
			x_pos + (0.5*width), y_pos - (0.5*height),
			x_pos - (0.5*width), y_pos + (0.5*height),
			x_pos + (0.5*width), y_pos - (0.5*height),
			x_pos + (0.5*width), y_pos + (0.5*height),
			x_pos - (0.5*width), y_pos + (0.5*height) };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program.positionAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
		glUseProgram(program.programID);
	}

	float x_pos;
	float y_pos;

	float width;
	float height;

	float velocity_x;
	float velocity_y;
};

bool Collision(const Entity& ball, const Entity& paddle) {
	if (ball.y_pos - ball.height / 2 > paddle.y_pos + paddle.height / 2) {
		return false;
	}
	else if (ball.y_pos + ball.height / 2 < paddle.y_pos - paddle.height / 2) {
		return false;
	}
	else if (ball.x_pos - ball.width / 2 > paddle.x_pos + paddle.width / 2) {
		return false;
	}
	else if (ball.x_pos + ball.width / 2 < paddle.x_pos - paddle.width / 2) {
		return false;
	}
	return true;
}

int main(int argc, char *argv[])
{
	//setup
	float width = 640 * 2;
	float length = 360 * 2;
	float lastFrameTicks = 0.0f;

	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, length, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif
	glViewport(0, 0, width, length);

	ShaderProgram program;
	program.Load(RESOURCE_FOLDER"vertex.glsl", RESOURCE_FOLDER"fragment.glsl");

	Matrix projectionMatrix;
	Matrix modelMatrix;
	Matrix viewMatrix;
	modelMatrix.Identity();
	projectionMatrix.SetOrthoProjection(-3.55, 3.55, -2.0f, 2.0f, -1.0f, 1.0f);
	program.SetProjectionMatrix(projectionMatrix);
	program.SetViewMatrix(viewMatrix);
	program.SetModelMatrix(modelMatrix);

	Entity leftPaddle(0.1, 0.5, -3.3, 0, 0, 3);
	Entity rightPaddle(0.1, 0.5, 3.3, 0, 0, 3);
	Entity ball(0.25, 0.25, 0, 0, 2, 2);
	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	SDL_Event event;
	bool done = false;
	while (!done) {
		float ticks = (float)SDL_GetTicks() / 1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
			// "R" to restart, space to change direction of ball
			else if (event.type == SDL_KEYDOWN) {
				if (event.key.keysym.scancode == SDL_SCANCODE_R) {
					ball.x_pos = 0;
					ball.y_pos = 0;
				}
				else if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
					ball.velocity_x *= -1;
				}
			}
		}
		//possible paddle inputs
		if (keys[SDL_SCANCODE_W]) {
			leftPaddle.y_pos += elapsed * leftPaddle.velocity_y;
		}
		else if (keys[SDL_SCANCODE_S]) {
			leftPaddle.y_pos -= elapsed * leftPaddle.velocity_y;
		}
		if (keys[SDL_SCANCODE_UP]) {
			rightPaddle.y_pos += elapsed * rightPaddle.velocity_y;
		}
		else if (keys[SDL_SCANCODE_DOWN]) {
			rightPaddle.y_pos -= elapsed * rightPaddle.velocity_y;
		}
		
		//keeps paddles from leaving screen
		if (leftPaddle.y_pos > 1.75) {
			leftPaddle.y_pos = 1.75;
		}

		else if (leftPaddle.y_pos < -1.75) {
			leftPaddle.y_pos = -1.75;
		}

		if (rightPaddle.y_pos > 1.75) {
			rightPaddle.y_pos = 1.75;
		}

		else if (rightPaddle.y_pos < -1.75) {
			rightPaddle.y_pos = -1.75;
		}
		
		//determines speed of ball
		ball.x_pos += elapsed * ball.velocity_x;
		ball.y_pos += elapsed * ball.velocity_y;
		
		//keeps ball from leaving through ceiling/floor
		if (ball.y_pos > 1.875 || ball.y_pos < -1.875) {
			ball.velocity_y = -ball.velocity_y;
		}
		
		//ball collision with paddles
		if (Collision(ball, rightPaddle) || Collision(ball, leftPaddle)) {
			ball.velocity_x = -ball.velocity_x;
		}

	

		glClear(GL_COLOR_BUFFER_BIT);
		//red if left wins, green if right wins
		program.SetColor(1, 1, 1, 1);
		ball.Draw(program);
		
		if (ball.x_pos>3.55){
			program.SetColor(1, 0, 0, 0);
		}
		leftPaddle.Draw(program);
		program.SetColor(1, 1, 1, 1);
		if (ball.x_pos < -3.55) {
			program.SetColor(0, 1, 0, 0);
		}
		rightPaddle.Draw(program);

		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}

