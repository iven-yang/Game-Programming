
#pragma once
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include "Matrix.h"
#include "ShaderProgram.h"
using namespace std;
#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

Matrix projectionMatrix;
Matrix viewMatrix;
Matrix modelMatrix;
ShaderProgram* program;

class Entity{
public:
	Entity(){};
	Entity(GLuint textureID0, float u0, float v0, float width, float height, float scale, bool dynamic, bool alive);

	void draw();
	void render();

	GLuint textureID;

	void resetcollision();

	bool collisionx(Entity entity);
	bool collisiony(Entity entity);
	bool collision(Entity entity);

	float u, v;
	float scale;
	float x, y;

	float elapsed;

	float width;
	float height;

	float v_x, v_y, a_x, a_y;

	float f_x, f_y;

	bool alive, dynamic;

	bool collidedTop, collidedBottom, collidedRight, collidedLeft;
};

Entity::Entity(GLuint textureID, float u0, float v0, float width, float height, float scale, bool dynamic = false, bool alive = false) :
textureID(textureID), width(width), height(height), scale(scale), dynamic(dynamic), alive(alive), a_y(0.0f), collidedTop(false), collidedBottom(false), collidedRight(false), collidedLeft(false){
	u=u0;
	v=v0;
};

void Entity::resetcollision(){
	collidedBottom = false;
	collidedLeft = false;
	collidedRight = false;
	collidedTop = false;
}

bool Entity::collision(Entity entity){
	float widthhalf1 = width*scale;
	float widthhalf2 = entity.width*entity.scale;
	float heighthalf1 = height*scale;
	float heighthalf2 = entity.height*entity.scale;
	float left1 = x - widthhalf1;
	float right1 = x + widthhalf1;
	float top1 = y + heighthalf1;
	float bottom1 = y - heighthalf1;
	float left2 = entity.x - widthhalf2;
	float right2 = entity.x + widthhalf2;
	float top2 = entity.y + heighthalf2;
	float bottom2 = entity.y - heighthalf2;
	if ((right1<left2) || (left1>right2) ||
		(top1<bottom2) || (bottom1>top2)){
		return false;
	}
	return true;
}

bool Entity::collisionx(Entity entity){
	float widthhalf1 = width*scale;
	float widthhalf2 = entity.width*entity.scale;
	float heighthalf1 = height*scale;
	float heighthalf2 = entity.height*entity.scale;
	float left1 = x - widthhalf1;
	float right1 = x + widthhalf1;
	float top1 = y + heighthalf1;
	float bottom1 = y - heighthalf1;
	float left2 = entity.x - widthhalf2;
	float right2 = entity.x + widthhalf2;
	float top2 = entity.y + heighthalf2;
	float bottom2 = entity.y - heighthalf2;
	if ((right1<left2) || (left1>right2) ||
		(top1<bottom2) || (bottom1>top2)){
		return false;
	}
	float xdist = fabs(x - entity.x);
	float x_pen = fabs(xdist - widthhalf1 - widthhalf2) + 0.0001f;
	if (right1 > right2) {
		collidedLeft = true;
		x += x_pen;
		return true;
	}
	else {
		x -= x_pen;
		collidedRight = true;
		return true;
	}
	return true;
}

bool Entity::collisiony(Entity entity){
	float widthhalf1 = width*scale;
	float widthhalf2 = entity.width*entity.scale;
	float heighthalf1 = height*scale;
	float heighthalf2 = entity.height*entity.scale;
	float left1 = x - widthhalf1;
	float right1 = x + widthhalf1;
	float top1 = y + heighthalf1;
	float bottom1 = y - heighthalf1;
	float left2 = entity.x - widthhalf2;
	float right2 = entity.x + widthhalf2;
	float top2 = entity.y + heighthalf2;
	float bottom2 = entity.y - heighthalf2;
	if ((right1<left2) || (left1>right2) ||
		(top1<bottom2) || (bottom1>top2)){
		return false;
	}
	float ydist = fabs(y - entity.y);
	float y_pen = fabs(ydist - heighthalf1 - heighthalf2) + 0.0001f;
	if (bottom1 > bottom2) {
		collidedBottom = true;
		y += y_pen;
		return true;
	}
	else {
		collidedTop = true;
		y -= y_pen;
		return true;
	}
}

void Entity::draw() {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glLoadIdentity();
	glTranslatef(x, y, 0.0f);

	GLfloat quad[] = { -width * scale, height * scale, -width * scale, -height * scale,
		width * scale, -height * scale, width * scale, height * scale };

	glVertexPointer(2, GL_FLOAT, 0, quad);
	glEnableClientState(GL_VERTEX_ARRAY);

	GLfloat quadUVs[] = { u, v, u, v + height, u + width, v + height, u + width, v };
	glTexCoordPointer(2, GL_FLOAT, 0, quadUVs);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDrawArrays(GL_QUADS, 0, 4);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
}

void Entity::render(){
	if (dynamic == false) {
		draw();
	}
	else if (alive == true){
		draw();
	}
}
