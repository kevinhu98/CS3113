#ifndef SheetSprite_h
#define SheetSprite_h
#include "ShaderProgram.h"

class SheetSprite {
public:
	SheetSprite(unsigned int textureID, float u, float v, float width, float height, float size);
	
	//for sheetsprites that are square patterned
	SheetSprite(unsigned int textureID, int index, int spriteX, int spriteY, float aspectRatio, float size);

	void Draw(ShaderProgram *program);

	float size;
	float aspectRatio;

	unsigned int textureID;
	float u;
	float v;
	float width;
	float height;
};

#endif // !SheetSprite.h
