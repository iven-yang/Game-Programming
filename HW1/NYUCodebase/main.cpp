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

SDL_Window *displayWindow;

GLuint LoadTexture(const char *image_path){
	SDL_Surface *surface = IMG_Load(image_path);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SDL_FreeSurface(surface);

	return textureID;
}

int main(int argc, char *argv[]){
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1200, 800, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	#ifdef _WINDOWS
		glewInit();
	#endif
	
	glClearColor(0.3f, 0.0f, 0.3f, 0.6f);
	glViewport(0, 0, 1200, 800);
	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");

	Matrix projection_matrix;
	Matrix model_matrix;
	Matrix view_matrix;

	projection_matrix.setOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);

	glUseProgram(program.programID);

	GLuint darth = LoadTexture(RESOURCE_FOLDER"DarthVaderUnicorn.png");
	GLuint fire = LoadTexture(RESOURCE_FOLDER"fire.png");
	GLuint yoda = LoadTexture(RESOURCE_FOLDER"YodaCat.png");

	float p1 = 0.0f;
	int p1i = 1;
	float p2 = 0.0f;
	int p2i = 0;
	float angle = 0.0f;
	float lastFrameTicks = 0.0f;

	glEnable(GL_BLEND);

	SDL_Event event;
	bool done = false;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
		}

		float ticks = (float)SDL_GetTicks()/1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		if (p1i){
			p1 += elapsed*2;
			if (p1 >= 3.2f)
				p1i = 0;
		}
		else{
			p1 -= elapsed*2;
			if (p1 <= -3.2f)
				p1i = 1;
		}
		
		if (p2i){
			p2 += elapsed*2;
			if (p2 >= 1.6f)
				p2i = 0;
		}
		else{
			p2 -= elapsed*2;
			if (p2 <= -1.6f)
				p2i = 1;
		}
		angle += elapsed;

		program.setModelMatrix(model_matrix);
		program.setProjectionMatrix(projection_matrix);
		program.setViewMatrix(view_matrix);

		glClear(GL_COLOR_BUFFER_BIT);

		model_matrix.identity();
		model_matrix.Rotate(angle);
		program.setModelMatrix(model_matrix);

		float p1vertices[] = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, p1vertices);
		glEnableVertexAttribArray(program.positionAttribute);

		float p1texCoords[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, p1texCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glBindTexture(GL_TEXTURE_2D, darth);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);


		model_matrix.identity();
		model_matrix.Translate(0.0f, p2, 0.0f);
		program.setModelMatrix(model_matrix);

		float p2vertices[] = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, p2vertices);
		glEnableVertexAttribArray(program.positionAttribute);

		float p2texCoords[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, p2texCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glBindTexture(GL_TEXTURE_2D, yoda);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);


		model_matrix.identity();
		model_matrix.Translate(p1, 0.0f, 0.0f);
		program.setModelMatrix(model_matrix);

		float plane_vertices[] = {-0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5};
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, plane_vertices);
		glEnableVertexAttribArray(program.positionAttribute);

		float plane_tex_coords[] = {0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0};
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, plane_tex_coords);
		glEnableVertexAttribArray(program.texCoordAttribute);

		glBindTexture(GL_TEXTURE_2D, fire);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}