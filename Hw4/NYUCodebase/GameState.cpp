#include "GameState.h"
#include <unordered_set>
#define GRAVITY 2.0f
#define ACCELERATION 0.5f
#define FRICTION 0.6f

GameState::GameState() {}
std::unordered_set<int> solidTiles = { 0,1,17,33 };
//std::unordered_set<int> solidTiles = { 0, 1, 2, 3, 6, 16, 17, 18, 19, 32, 33, 34, 35, 100, 101};

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
		player = new Entity(x, y, &sheetSprites[0], PLAYER);
		entities.push_back(player);
	}
	else if (type == "ENEMY") {		
		Entity* enemy = new Entity(x, y, &sheetSprites[0], ENEMY);
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
				player->y_velocity = 1.4f;
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
		entities[i]->y_velocity = lerp(entities[i]->y_velocity, 0.0f, elapsed * FRICTION);

		//acceleration
		entities[i]->x_velocity += entities[i]->x_acceleration * elapsed;
		entities[i]->y_velocity += entities[i]->y_acceleration * elapsed;

		//gravity
		entities[i]->y_velocity -= elapsed * GRAVITY;

		//x-velo
		entities[i]->x_pos += entities[i]->x_velocity * elapsed;
		CollideWithMapX(*entities[i]);

		//y-velo
		entities[i]->y_pos += entities[i]->y_velocity * elapsed;
		CollideWithMapY(*entities[i]);

		//enemy movement
		if (entities[i]->type == ENEMY) {
			if (entities[i]->collidedLeft || entities[i]->collidedRight) {
				entities[i]->x_acceleration *= -1;
			}
		}

		//death / enemy collision
		if (player->collisionEntity(entities[i]) && entities[i]->type == ENEMY) {
			player->x_pos = map->tileSize / 2;
			player->y_pos = 0.2;
			player->x_velocity = 0;
			
		}
	}
	//bounds check + keeps in bounds
	if (player->x_pos - player->width / 2 < 0) {
		player->x_velocity = 0;
		player->x_pos = map->tileSize / 2;
	}
	else if (player->x_pos + player->width / 2 > (map->tileSize*map->mapWidth)) {
		player->x_velocity = 0;
		player->x_pos = map->tileSize*map->mapWidth - map->tileSize / 2;
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

bool GameState::ResolveCollisionInX(Entity& entity, int x, int y, float size) {
	if (x < 0 || y < 0 || map->mapData[y][x] == 0 ||
		solidTiles.find(map->mapData[y][x] - 1) == solidTiles.end()) {
		return false;
	}
	//collided tile center
	float tileCenter = (x * size) + (size / 2);
	return entity.collisionInX(tileCenter, size);
}

bool GameState::ResolveCollisionInY(Entity& entity, int x, int y, float size) {
	if (x < 0 || y < 0 || map->mapData[y][x] == 0 ||
		solidTiles.find(map->mapData[y][x] - 1) == solidTiles.end()) {
		return false;
	}
	//collided tile center
	float tileCenter = (-y * size) - (size / 2);
	return entity.collisionInY(tileCenter, size);
}

void GameState::CollideWithMapX(Entity& entity) {
	if (entity.x_velocity > 0) {
		//right collision
		int rightX, rightBottomY, rightTopY; //two points on right-line of entity
		map->worldToTileCoordinates(entity.x_pos + entity.width / 2, entity.y_pos + entity.height/2.1, rightX, rightBottomY);
		map->worldToTileCoordinates(entity.x_pos + entity.width / 2, entity.y_pos - entity.height/2.1, rightX, rightTopY);
		//ResolveCollisionInX(entity, rightX, rightBottomY, map->tileSize);
		ResolveCollisionInX(entity, rightX, rightTopY, map->tileSize);
	}
	else {
		//left collision
		int leftX, leftBottomY, leftTopY; //two points on left-line of entity
		map->worldToTileCoordinates(entity.x_pos - entity.width / 2, entity.y_pos + entity.height / 2.1, leftX, leftBottomY);
		map->worldToTileCoordinates(entity.x_pos - entity.width / 2, entity.y_pos - entity.height / 2.1, leftX, leftTopY);
		//ResolveCollisionInX(entity, leftX, leftBottomY, map->tileSize);
		ResolveCollisionInX(entity, leftX, leftTopY, map->tileSize);
		}
}

void GameState::CollideWithMapY(Entity& entity) {
	int entityP1 = map->getTileCoordinateXPos(entity.x_pos - entity.width / 3);
	int entityP2 = map->getTileCoordinateXPos(entity.x_pos + entity.width / 3);
	
	if (entity.y_velocity > 0) {
		int topY = map->getTileCoordinateYPos(entity.y_pos + entity.height / 2);
		if (!ResolveCollisionInY(entity, entityP1, topY, map->tileSize)) {
			ResolveCollisionInY(entity, entityP2, topY, map->tileSize);
		}
	}
	else {
		int botY = map->getTileCoordinateYPos(entity.y_pos - entity.height / 2);
		if (!ResolveCollisionInY(entity, entityP1, botY, map->tileSize))
			ResolveCollisionInY(entity, entityP2, botY, map->tileSize);
		}
	}