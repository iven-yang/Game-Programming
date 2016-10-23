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

class paddle{
public:
	paddle(float l,float u,float d,float r){
		left = l;
		up = u;
		down = d;
		right = r;
	}
	float left, up, down, right;
};

class ball{
public:
	ball(float v1, float v2){
		x = 0;
		y = 0;
		vx = v1;
		vy = v2;
		//goingUp = true;
		//goingRight = true;
	}
	void move(float e){
		//if (goingRight)
			x += vx*e;
		//else
			//x -= vx*e;
		//if (goingUp)
			y += vy*e;
		//else
			//y -= vy*e;
	}
	void ret(){
		x = 0;
		y = 0;
	}
	float x, y, vx, vy;
	//bool goingUp, goingRight;
};

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
	displayWindow = SDL_CreateWindow("Pong - Player 1 use w and s - Player 2 use up and down arrows", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1200, 800, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	#ifdef _WINDOWS
		glewInit();
	#endif
	
	glClearColor(0.3f, 0.0f, 0.3f, 0.6f);
	glViewport(0, 0, 1200, 800);
	ShaderProgram program(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	glUseProgram(program.programID);



	Matrix projection_matrix;
	Matrix model_matrix;
	Matrix view_matrix;

	projection_matrix.setOrthoProjection(-3.55f, 3.55f, -2.0f, 2.0f, -1.0f, 1.0f);

	Matrix pad1_matrix;
	Matrix pad2_matrix;
	Matrix bal_matrix;

	GLuint darth = LoadTexture(RESOURCE_FOLDER"DarthVaderUnicorn.png");
	GLuint fire = LoadTexture(RESOURCE_FOLDER"Earth.png");
	GLuint yoda = LoadTexture(RESOURCE_FOLDER"YodaCat.png");

	paddle pad1(-3.5f, 0.5f, -0.5f, -3.4f);
	paddle pad2(3.4f, 0.5f, -0.5f, 3.5f);
	ball bal(1.0f, 1.0f);

	glEnable(GL_BLEND);

	SDL_Event event;
	bool done = false;
	float lastFrameTicks = 0.0f;

	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
				done = true;
			}
			if (event.type == SDL_KEYDOWN){
				if (event.key.keysym.scancode == SDL_SCANCODE_W){
					if (pad1.up < 2.0f){
						pad1.up += 0.1f;
						pad1.down += 0.1f;
						pad1_matrix.Translate(0.0f, 0.1f, 0.0f);
					}
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_S){
					if (pad1.down > -2.0f){
						pad1.up -= 0.1f;
						pad1.down -= 0.1f;
						pad1_matrix.Translate(0.0f, -0.1f, 0.0f);
					}
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_UP){
					if (pad2.up < 2.0f){
						pad2.up += 0.1f;
						pad2.down += 0.1f;
						pad2_matrix.Translate(0.0f, 0.1f, 0.0f);
					}
				}
				if (event.key.keysym.scancode == SDL_SCANCODE_DOWN){
					if (pad2.down > -2.0f){
						pad2.up -= 0.1f;
						pad2.down -= 0.1f;
						pad2_matrix.Translate(0.0f, -0.1f, 0.0f);
					}
				}
			}
		}

		float ticks = (float)SDL_GetTicks()/1000.0f;
		float elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		program.setProjectionMatrix(projection_matrix);
		program.setViewMatrix(view_matrix);

		glClear(GL_COLOR_BUFFER_BIT);

		float plane_tex_coords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
		//paddle 1
		program.setModelMatrix(pad1_matrix);
		float pad1_coords[] = { -3.5f, -0.5f, -3.4f, -0.5f, -3.4f, 0.5f, -3.4f, 0.5f, -3.5f, 0.5f, -3.5f, -0.5f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, pad1_coords);
		glEnableVertexAttribArray(program.positionAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, plane_tex_coords);
		glEnableVertexAttribArray(program.texCoordAttribute);
		glBindTexture(GL_TEXTURE_2D, darth);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		// paddle 2
		program.setModelMatrix(pad2_matrix);
		float pad2_coords[] = { 3.4f, -0.5f, 3.5f, -0.5f, 3.5f, 0.5f, 3.5f, 0.5f, 3.4f, 0.5f, 3.4f, -0.5f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, pad2_coords);
		glEnableVertexAttribArray(program.positionAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, plane_tex_coords);
		glEnableVertexAttribArray(program.texCoordAttribute);
		glBindTexture(GL_TEXTURE_2D, yoda);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);

		// ball
		program.setModelMatrix(bal_matrix);
		float ball_coords[] = { -0.1f, -0.1f, 0.1f, -0.1f, 0.1f, 0.1f, 0.1f, 0.1f, -0.1f, 0.1f, -0.1f, -0.1f };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, ball_coords);
		glEnableVertexAttribArray(program.positionAttribute);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, plane_tex_coords);
		glEnableVertexAttribArray(program.texCoordAttribute);
		glBindTexture(GL_TEXTURE_2D, fire);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);


		//Win detection
		if (bal.x + 0.1f > pad2.right)
		{
			done = true;
			std::cout << "Player 1 Wins";
		}
		if (bal.x - 0.1f < pad1.left){
			done = true;
			std::cout << "Player 2 Wins";
		}

		//Screen hit detection
		if (bal.y + 0.1f >= 2.0f || bal.y - 0.1f <= -2.0f){
			bal.vy *= -1;
		}

		//Paddle hit detection
		if (!((bal.y-0.1f > pad1.up) || (bal.y+0.1f < pad1.down) || (bal.x-0.1f > pad1.right) || (bal.x+0.1f < pad1.left)) ||
			!((bal.y-0.1f > pad2.up) || (bal.y+0.1f < pad2.down) || (bal.x-0.1f > pad2.right) || (bal.x+0.1f < pad2.left))){
			bal.vx *= -1;
			/*for (int i = 0; i < 2; ++i){
				bal.move(elapsed);
				bal_matrix.Translate(bal.vx*elapsed, bal.vy*elapsed, 0.0f);
			}*/
		}
		bal.move(elapsed);
		bal_matrix.Translate(bal.vx*elapsed, bal.vy*elapsed, 0.0f);

		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
	return 0;
}