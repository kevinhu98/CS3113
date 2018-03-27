#ifndef GameState_h
#define GameState_h

#include "Entity.h"
//#include <vector>

class GameState {
public:
	GameState();
	void Initialize(SheetSprite* player, SheetSprite* bullet, SheetSprite* enemy);
	void Render();
	void Update(float elapsed);
	void ProcessInput();
	void ShootBullet();
	
	Entity* player;
	std::vector<Entity> enemies;
	std::vector<Entity> bullets; 
	int bulletIndex = 0;
	
}; 

#endif
