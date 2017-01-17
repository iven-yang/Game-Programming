#ifndef Entity_h
#define Entity_h

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <cstdlib>
#include <ctime>
#include <vector>

#include "ShaderProgram.h"
#include "Matrix.h"
#include "Utils.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

class Entity {
public:
	Matrix entityMatrix;
	float position[2];		//location (center point of entity)
	float boundaries[4];	//top, bottom, left, right (from position)
	float size[2];

	float speed[2];
	float acceleration[2];
	bool collided[4]; //same as boundaries, top bot left right

	bool isStatic = true;
	Type type;

	float u;
	float v;
	float width;//1 is facing right, -1 is facing left
	float height;
	std::vector<GLuint> texture;
	int currT;
	float counter = 0;
	bool inAir = false;
	bool attacking = false;
	bool gettingWrecked = false;
	bool dead = false;
	float cooldown = 0;

	Entity();
	Entity(float x, float y, float spriteU, float spriteV, float spriteWidth, float spriteHeight, float dx, float dy, std::vector<GLuint> spriteTexture, Type newType);
	Entity(float x, float y, float spriteU, float spriteV, float spriteWidth, float spriteHeight, float dx, float dy, std::vector<GLuint> spriteTexture, float sizeX, float sizeY, Type newType);
	void draw(ShaderProgram* program);
	void update(float elapsed);
	void updateX(float elapsed);
	void updateY(float elapsed);
	void animate(float elapsed);
};


#endif