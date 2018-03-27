#ifndef SheetSprite_h
#define SheetSprite_h
#include "ShaderProgram.h"

class SheetSprite {
public:
	SheetSprite(unsigned int textureID, float u, float v, float width, float height, float size);
	
	void Draw(ShaderProgram *program);

	float size;
	unsigned int textureID;
	float u;
	float v;
	float width;
	float height;
};

#endif // !SheetSprite.h
