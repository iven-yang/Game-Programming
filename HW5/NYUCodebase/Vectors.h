
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
#include <time.h>
#include <math.h>

using namespace std;

class Vectors {
public:
	Vectors(float x, float y) :x(x), y(y) {}

	void rotate(float xx, float yy, float angle);

	float x, y;
};

void Vectors::rotate(float xx, float yy, float angle){
	float sign = sin(angle);
	float cosign = cos(angle);
	x -= xx;
	y -= yy;

	float xf = x*cosign - y*sign;
	float yf = x*sign + y*cosign;
	x = xf + xx;
	y = yf + yy;
}
