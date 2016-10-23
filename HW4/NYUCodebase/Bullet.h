
#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>

#include "Matrix.h"
#include "ShaderProgram.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

class Bullet{
public:
	void draw();
	void update();

	Bullet(){};
	Bullet(GLuint textureID, float u, float v, float width, float height, float scale, float x, float y, bool visible) :textureID(textureID), u(u), v(v), width(width), height(height), scale(scale), x(x), y(y), speed(0.8f), visible(visible){};

	float u, v, width, height, scale, x, y, speed, timeAlive;
	GLuint textureID;
	bool visible;
};

void Bullet::draw(){
	if (visible){
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
}