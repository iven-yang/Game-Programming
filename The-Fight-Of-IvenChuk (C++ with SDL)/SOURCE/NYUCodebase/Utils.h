#ifndef Utils_h
#define Utils_h

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <cstdlib>
#include <ctime>
#include <vector>
#include <SDL_mixer.h>

#include "ShaderProgram.h"
#include "Matrix.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

enum Type { PLAYER, BLOCK, WIZARD };

class Ut {
public:
	void DrawText(ShaderProgram* program, int fontTexture, std::string text, float size, float spacing);
	GLuint LoadTexture(const char* image_path);
	float map(float x, float in_min, float in_max, float out_min, float out_max);
	void refresh(Matrix projectionMatrix, Matrix viewMatrix, Matrix modelMatrix, ShaderProgram* program);
};

#endif
