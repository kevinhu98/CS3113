#include "GameState.h"
#include "Entity.h"
#include "main.h"

#define MAX_BULLETS 50
int bulletIndex = 0;

GameState::GameState() : player(new Entity()), enemies(27, Entity()), bullets(MAX_BULLETS, Entity()) {}

void GameState::Initialize(SheetSprite* playerSprite, SheetSprite* bulletSprite, SheetSprite* enemySprite) {
	player->type = PLAYER;
	player->x_pos = 0.0f;
	player->y_pos = -1.7f;
	player->width = 1.0f;
	player->height = 1.0f;
	player->x_velocity = 1.0f;
	player->alive = true;
	player->setSprite(playerSprite);

	//move all bullets out of screen
	for (int i = 0; i < MAX_BULLETS; ++i) {
		bullets[i].x_pos = 1.0f;
		bullets[i].y_pos = 1000.0f;
		bullets[i].x_velocity = 0.0f;
		bullets[i].y_velocity = 3.0f;
		bullets[i].type = BULLET;
		bullets[i].alive = true;
		bullets[i].setSprite(bulletSprite);
	}

	//Position enemies

	float invader_x = -2.1f;
	float invader_y = 1.6f;
	for (int i = 0; i < enemies.size(); ++i) {
		enemies[i].type = ENEMY;
		enemies[i].alive = true;
		enemies[i].x_velocity = 0.5;
		enemies[i].y_velocity = 0;
		enemies[i].setSprite(enemySprite);
		if (invader_x > 2.2) {
			invader_x = -2.1f;
			invader_y -= 0.4;
			
		}
		enemies[i].x_pos = invader_x;
		enemies[i].y_pos = invader_y;
		invader_x += 0.5f;

	}
}

void GameState::ShootBullet() {
	bullets[bulletIndex].x_pos = player->x_pos;
	bullets[bulletIndex].y_pos = player->y_pos + 0.3;
	bulletIndex++;
	if (bulletIndex > MAX_BULLETS - 1) {
		bulletIndex = 0;
	}
};

void GameState::ProcessInput() {
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			done = true;
		}
		else if (event.type == SDL_KEYDOWN) {
			if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
				ShootBullet();
			}
			//Alternate exit button
			if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
				done = true;
			}
		}
	}

	if (keys[SDL_SCANCODE_LEFT] && player->x_pos > -3.35) {
		player->x_velocity = -1;
	}
	else if (keys[SDL_SCANCODE_RIGHT] && player->x_pos < 3.55) {
		player->x_velocity = 1;
	}
	else {
		player->x_velocity = 0;
	}
}

void GameState::Update(float elapsed) {
	player->update(elapsed);

	//move bullets and check for collision to enemies
	for (int i = 0; i < MAX_BULLETS; i++) {
		bullets[i].update(elapsed);

		for (int j = 0; j < enemies.size(); j++) {
			if (enemies[j].Collision(bullets[i]) == true) {
				enemies[j].die();
				bullets[i].y_pos = 100000;
			}
		}
	}

	//moves enemies left, right, and down
	for (size_t i = 0; i < enemies.size(); i++) {
		enemies[i].update(elapsed);
		
		if (enemies[i].x_pos > 3.3f || enemies[i].x_pos < -3.3f) {
			for (int j = 0; j < enemies.size(); j++) {
				enemies[j].x_velocity *= -1;
				enemies[j].y_pos -= 0.05;
				enemies[j].update(elapsed);
			}
			break;
		}
	}
	//defeat
	for (size_t i = 0; i < enemies.size(); i++) {
		if (enemies[i].alive == true && enemies[i].y_pos < -3.55) {
			mode = STATE_GAME_OVER;
		}
	}
}

void GameState::Render() {
	player->render(program, modelMatrix);

	for (size_t i = 0; i < enemies.size(); ++i) {
		if (enemies[i].alive == true) {
			enemies[i].render(program, modelMatrix);
		}
	}

	for (size_t i = 0; i < bullets.size(); ++i) {
		bullets[i].render(program, modelMatrix);
	}
}

void GameState::reset() {
	player->x_pos = 0.0f;
	player->y_pos = -1.7f;
	player->alive = true;

	//Reposition enemies

	float invader_x = -2.1f;
	float invader_y = 1.6f;
	for (int i = 0; i < enemies.size(); ++i) {
		enemies[i].alive = true;
		if (invader_x > 2.2) {
			invader_x = -2.1f;
			invader_y -= 0.4;

		}
		enemies[i].x_pos = invader_x;
		enemies[i].y_pos = invader_y;
		invader_x += 0.5f;

	}
}

