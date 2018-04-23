#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include "FlareMap.h"
#include "ShaderProgram.h"
#include <fstream>
#include <sstream>
#include <cassert>
#include <SDL_opengl.h>


FlareMap::FlareMap() {
	mapData = nullptr;
	mapWidth = 0;
	mapHeight = 0;
	tileSize = 0.2;
}

FlareMap::~FlareMap() {
	for (int i = 0; i < mapHeight; i++) {
		delete mapData[i];
	}
	delete mapData;
}

void FlareMap::render(ShaderProgram& shader) {
	std::vector<float> vertexData;
	std::vector<float> texCoordData;
	for (int y = 0; y < mapHeight; y++) {
		for (int x = 0; x < mapWidth; x++) {
			
			if (mapData[y][x] == 0) continue;
			
			float u = (float)((mapData[y][x] - 1) % spritesX) / (float)spritesX;
			float v = (float)((mapData[y][x] - 1) / spritesX) / (float)spritesY;
			float spriteWidth = 1.0f / (float)spritesX;
			float spriteHeight = 1.0f / (float)spritesY;
			vertexData.insert(vertexData.end(), {
				tileSize * x, -tileSize * y,
				tileSize * x, (-tileSize * y) - tileSize,
				(tileSize * x) + tileSize, (-tileSize * y) - tileSize,

				tileSize * x, -tileSize * y,
				(tileSize * x) + tileSize, (-tileSize * y) - tileSize,
				(tileSize * x) + tileSize, -tileSize * y
				});
		
			texCoordData.insert(texCoordData.end(), {
				u, v,
				u, v + spriteHeight,
				u + spriteWidth, v + spriteHeight,

				u, v,
				u + spriteWidth, v + spriteHeight,
				u + spriteWidth, v
				});
		}
	}

	glBindTexture(GL_TEXTURE_2D, spriteSheetTexture);
	glVertexAttribPointer(shader.positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(shader.positionAttribute);
	glVertexAttribPointer(shader.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(shader.texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, (int)vertexData.size() / 2);

	glDisableVertexAttribArray(shader.positionAttribute);
	glDisableVertexAttribArray(shader.texCoordAttribute);
}

void FlareMap::setSpriteSheet(int sheet, int spriteX, int spriteY) {
	this->spritesX = spriteX;
	this->spritesY = spriteY;
	spriteSheetTexture = sheet;
}

bool FlareMap::ReadHeader(std::ifstream &stream) {
	std::string line;
	mapWidth = -1;
	mapHeight = -1;
	while (std::getline(stream, line)) {
		if (line == "") { break; }
		std::istringstream sStream(line);
		std::string key, value;
		std::getline(sStream, key, '=');
		std::getline(sStream, value);
		if (key == "width") {
			mapWidth = std::atoi(value.c_str());
		}
		else if (key == "height") {
			mapHeight = std::atoi(value.c_str());
		}
	}
	if (mapWidth == -1 || mapHeight == -1) {
		return false;
	}
	else {
		mapData = new unsigned int*[mapHeight];
		for (int i = 0; i < mapHeight; ++i) {
			mapData[i] = new unsigned int[mapWidth];
		}
		return true;
	}
}

bool FlareMap::ReadLayerData(std::ifstream &stream) {
	std::string line;
	while (getline(stream, line)) {
		if (line == "") { break; }
		std::istringstream sStream(line);
		std::string key, value;
		std::getline(sStream, key, '=');
		std::getline(sStream, value);
		if (key == "data") {
			for (int y = 0; y < mapHeight; y++) {
				getline(stream, line);
				std::istringstream lineStream(line);
				std::string tile;
				for (int x = 0; x < mapWidth; x++) {
					std::getline(lineStream, tile, ',');
					unsigned int val = atoi(tile.c_str());
					mapData[y][x] = val;
				}
			}
		}
	}
	return true;
}


bool FlareMap::ReadEntityData(std::ifstream &stream) {
	std::string line;
	std::string type;
	while (getline(stream, line)) {
		if (line == "") { break; }
		std::istringstream sStream(line);
		std::string key, value;
		getline(sStream, key, '=');
		getline(sStream, value);
		if (key == "type") {
			type = value;
		}
		else if (key == "location") {
			std::istringstream lineStream(value);
			std::string xPosition, yPosition;
			getline(lineStream, xPosition, ',');
			getline(lineStream, yPosition, ',');

			FlareMapEntity newEntity;
			newEntity.type = type;
			newEntity.x = std::atoi(xPosition.c_str());
			newEntity.y = std::atoi(yPosition.c_str());
			entities.push_back(newEntity);
		}
	}
	return true;
}

void FlareMap::Load(const std::string fileName) {
	std::ifstream infile(fileName);
	if (infile.fail()) {
		assert(false); // unable to open file
	}
	std::string line;
	while (std::getline(infile, line)) {
		if (line == "[header]") {
			if (!ReadHeader(infile)) {
				assert(false); // invalid file data
			}
		}
		else if (line == "[layer]") {
			ReadLayerData(infile);
		}
		else if (line == "[ObjectsLayer]") {
			ReadEntityData(infile);
		}
	}
} 

void FlareMap::worldToTileCoordinates(float worldX, float worldY, int &gridX, int &gridY) {
	gridX = (int)(worldX / tileSize);
	gridY = (int)(-worldY / tileSize);
}

int FlareMap::getTileCoordinateXPos(float worldX) {
	return (int)(worldX / tileSize);
}

int FlareMap::getTileCoordinateYPos(float worldY) {
	return (int)(-worldY / tileSize);
}
