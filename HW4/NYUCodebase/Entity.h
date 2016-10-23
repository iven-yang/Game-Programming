
#include "Bullet.h"

class Entity{
public:
	void draw(float spacing = 1.0f);
	void render();
	Entity(){};
	Entity(GLuint textureID, float u, float v, float width, float height, float scale) :textureID(textureID), u(u), v(v), width(width), height(height), scale(scale), alive(true), vx(0.1f), vy(0.02f), shot(false), pauseoverlay(-999.0f){};

	GLuint textureID;
	float u, v, width, height, scale, x, y, vx, vy, pauseoverlay;
	bool alive, shot;
	Bullet bullet;
};

void Entity::draw(float spacing) {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glLoadIdentity();
	glTranslatef(x, y, 0.0f);
	glScalef(spacing, 1.0f, 1.0f);

	GLfloat quad[] = { -width * scale, height * scale, -width * scale, -height * scale, width * scale, -height * scale, width * scale, height * scale };
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
	if (alive == true)
		draw();
}
