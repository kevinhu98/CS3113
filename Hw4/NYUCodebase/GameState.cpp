#include "GameState.h"

#define GRAVITY 1.0f
#define ACCELERATION 0.5f
#define FRICTION 0.6f

GameState::GameState() {}

std::vector<int> solidTiles = { 0, 2, 18, 34};

void GameState::Initialize(GameUtilities* utilities, FlareMap* map) {
	this->Utilities = utilities;
	this->map = map;
	sheetSprites.emplace_back(map->spriteSheetTexture, 80, map->spritesX, map->spritesY, 1.0f, map->tileSize);
	for (size_t i = 0; i < map->entities.size(); i++) {
		placeEntity(map->entities[i].type, map->entities[i].x * map->tileSize + map->tileSize / 2, (map->entities[i].y - 1) * -map->tileSize - map->tileSize / 2);
	}
}

void GameState::placeEntity(std::string type, float x, float y) {
	if (type == "PLAYER") {
		//entities.emplace_back(x, y, &sheetSprites[0], PLAYER);
		player = new Entity(x, y, &sheetSprites[0], PLAYER);
		entities.push_back(player);
	}
	else if (type == "ENEMY") {		
		Entity* enemy = new Entity(x, y, &sheetSprites[0], PLAYER);
		entities.push_back(enemy);
	}
}

void GameState::ProcessInput() {
	SDL_Event& event = *Utilities->event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			*Utilities->done = true;
		}
		else if (event.type == SDL_KEYDOWN) {
			if (event.key.keysym.scancode == SDL_SCANCODE_SPACE && player->collidedBottom) {
				player->y_velocity = 5.0f;
			}
			//Alternate exit button
			if (event.key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
				*Utilities->done = true;
			}
		}
		else if (event.type == SDL_KEYUP) {
			if (event.key.keysym.scancode == SDL_SCANCODE_UP) {
				player->x_acceleration = 0.0f;
			}
			else if (event.key.keysym.scancode == SDL_SCANCODE_LEFT || event.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
				player->x_acceleration = 0.0f;
			}
		}
	}
	const Uint8* keys = Utilities->keys;

	if (player->collidedBottom) {
		if (keys[SDL_SCANCODE_LEFT]) {
			player->x_acceleration = -0.7f;
		}
		else if (keys[SDL_SCANCODE_RIGHT]) {

			player->x_acceleration = 0.7f;
		}
	}
	else {
		if (keys[SDL_SCANCODE_RIGHT]) {
			player->x_velocity = 1.0f;
		}
		else if (keys[SDL_SCANCODE_LEFT]) {
			player->x_velocity = -1.0f;
		}
	}
	
}

void GameState::Update(float elapsed) {
	for (size_t i = 0; i < entities.size(); ++i) {
		entities[i]->update(elapsed);
		
		//friction
		entities[i]->x_velocity = lerp(entities[i]->x_velocity, 0.0f, elapsed * FRICTION);
		//entities[i].y_velocity = lerp(entities[i].y_velocity, 0.0f, lerp3);

		//acceleration
		entities[i]->x_velocity += entities[i]->x_acceleration * elapsed;
		//entities[i].y_velocity += entities[i].y_acceleration * elapsed;

		//gravity
		//entities[i].y_velocity -= elapsed * GRAVITY;

		//x-velo
		entities[i]->x_pos += entities[i]->x_velocity * elapsed;
		//CollideWithMapX(entities[i]);

		//y-velo
		//entities[i].y_pos += entities[i].y_velocity * elapsed;
		//CollideWithMapY(entities[i]);
	}
	
}

void GameState::Render() {
	ShaderProgram& program = *Utilities->shader;
	modelMatrix.Identity();
	program.SetModelMatrix(modelMatrix);

	//transitions screen
	viewMatrix.Identity();
	if (player) {
		viewMatrix.Translate(-player->x_pos, -player->y_pos, 0.0f);
		program.SetViewMatrix(viewMatrix);
	}
	//render map
	map->render(program);

	//render entities
	for (size_t i = 0; i < entities.size(); ++i) {
		entities[i]->render(program);
	}
}

void GameState::CollideWithMapX(Entity& entity) {
	if (entity.x_velocity > 0) {
		int rightX, rightY;
		map->worldToTileCoordinates(entity.x_pos + entity.width / 2, entity.y_pos, rightX, rightY);
		//check tile solid
		if (map->mapData[rightY][rightX] == 0, 2, 18, 34) {
			//get collision tile center
			float tileCenter = rightX * map->tileSize + map->tileSize / 2;
			entity.collisionInX(tileCenter, map->tileSize);
		}
	}
	else {
		int leftX, leftY;
		map->worldToTileCoordinates(entity.x_pos - entity.width / 2, entity.y_pos, leftX, leftY);
		if (map->mapData[leftY][leftX] == 0, 2, 18, 34) {
			float tileCenter = leftX * map->tileSize + map->tileSize / 2;
			entity.collisionInX(tileCenter, map->tileSize);
		}
	}
}

void GameState::CollideWithMapY(Entity& entity) {
	//two points on x of entity 
	int entityP1 = map->getTileCoordinateXPos(entity.x_pos - entity.width / 3);
	//int entityP2 = map->getTileCoordinateXPos(entity.x_pos - entity.width / 3);

	if (entity.y_velocity > 0) {
		int topY = map->getTileCoordinateYPos(entity.y_pos + entity.height / 2);
		if (map->mapData[entityP1][topY] == 0, 2, 18, 34) {
			float tileCenter = -topY * map->tileSize - map->tileSize / 2;
			entity.collisionInY(tileCenter, map->tileSize);
		}
	}
	else {
		int botY = map->getTileCoordinateYPos(entity.y_pos - entity.height / 2);
		if (map->mapData[entityP1][botY] == 0, 2, 18, 34) {
			float tileCenter = -botY * map->tileSize - map->tileSize / 2;
			entity.collisionInY(tileCenter, map->tileSize);
		}
	}
}