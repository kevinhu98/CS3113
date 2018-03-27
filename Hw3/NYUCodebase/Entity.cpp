#include "Entity.h"
#include "SheetSprite.h"
#include "Matrix.h"


Entity::Entity() {}
Entity::~Entity() {}

Entity::Entity(float x_pos, float y_pos, float width, float height, SheetSprite* sprite, EntityType type) :
	x_pos(x_pos), y_pos(y_pos), width(width), height(height), sprite(sprite), type(type), alive(true) {}

void Entity::change_x(float distance_x) {
	x_pos += distance_x;
}

void Entity::change_y(float distance_y) {
	y_pos += distance_y;
}

void Entity::die() {
	alive = false;
}

void Entity::setSprite(SheetSprite* newSprite) {
	sprite = newSprite;
	float aspectRatio = newSprite->width / newSprite->height;
	width = aspectRatio * newSprite->size;
	height = newSprite->size;
	
}

void Entity::render(ShaderProgram &program, Matrix& modelMatrix) {
	//if (!alive) return;
	modelMatrix.Identity();
	modelMatrix.Translate(x_pos, y_pos, 0.0f);

	if (this->alive == true) {
		program.SetModelMatrix(modelMatrix);
		sprite->Draw(&program);
	}
}

void Entity::update(float elapsed) {
	x_pos += x_velocity * elapsed;
	y_pos += y_velocity * elapsed;

}

bool Entity::Collision(Entity& bullet) {
	if (this->alive == false) {
		return false;
	}
	if (y_pos - height / 2 > bullet.y_pos + bullet.height / 2) {
		return false;
	}
	else if (y_pos + height / 2 < bullet.y_pos - bullet.height / 2) {
		return false;
	}
	else if (x_pos - width / 2 > bullet.x_pos + bullet.width / 2) {
		return false;
	}
	else if (x_pos + width / 2 < bullet.x_pos - bullet.width / 2) {
		return false;
	}
	return true;
	
}
