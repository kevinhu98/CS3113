#ifndef Entity_h
#define Entity_h

#include <vector>
#include "ShaderProgram.h"
#include "SheetSprite.h"

enum EntityType{ PLAYER, ENEMY, BULLET};

class Entity {
public:
	Entity();
	Entity(float x_pos, float y_pos, float width, float height, SheetSprite* sprite, EntityType type);
	~Entity();
	
	void change_x(float distance_x);
	void change_y(float distance_y);
	void die();
	void setSprite(SheetSprite* newSprite);
	void render(ShaderProgram &program, Matrix& modelMatrix);
	void update(float elapsed);
	bool Collision(Entity& bullet);
	//void shoot();

	SheetSprite* sprite;
	EntityType type;
	float x_pos;
	float y_pos;
	float x_velocity;
	float y_velocity;
	float width;
	float height;
	
	bool alive;
};


#endif
