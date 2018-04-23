#ifndef FlareMap_h
#define FlareMap_h

#include <string>
#include <stdio.h>
#include <vector>

class ShaderProgram;

struct FlareMapEntity {
	std::string type;
	float x;
	float y;
};

class FlareMap {
public:
	FlareMap();
	~FlareMap();

	void Load(const std::string fileName);
	void setSpriteSheet(int sheet, int spriteX, int spriteY);
	void render(ShaderProgram& shader);
	int mapWidth;
	int mapHeight;
	
	int spriteSheetTexture;
	int spritesX;
	int spritesY;
	float tileSize;

	void worldToTileCoordinates(float worldX, float worldY, int &gridX, int &gridY);
	int FlareMap::getTileCoordinateXPos(float worldX);
	int FlareMap::getTileCoordinateYPos(float worldY);
	unsigned int **mapData;
	std::vector<FlareMapEntity> entities;

private:
	bool ReadHeader(std::ifstream &stream);
	bool ReadLayerData(std::ifstream &stream);
	bool ReadEntityData(std::ifstream &stream);
};

#endif