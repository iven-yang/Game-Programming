
#include "ShaderProgram.h"
#include "Matrix.h"
#include "Entity.h"

SDL_Window* displayWindow;
GLuint groundTexture;
GLuint fontTexture;

float texture_coords[] = { 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f };

float lastFrameTicks = 0.0f;
float elapsed;
#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6

