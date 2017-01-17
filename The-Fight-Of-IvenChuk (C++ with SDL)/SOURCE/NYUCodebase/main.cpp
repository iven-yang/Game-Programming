#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_image.h>
#include <cstdlib>
#include <stdlib.h>
#include <math.h>
#include <ctime>
#include <vector>

#include "ShaderProgram.h"
#include "Matrix.h"
#include "Utils.h"
#include "Entity.h"

#ifdef _WINDOWS
#define RESOURCE_FOLDER ""
#else
#define RESOURCE_FOLDER "NYUCodebase.app/Contents/Resources/"
#endif

// GLOBAL GAME VARIABLES____________________________________________________________________________________________________________________________
int MOA = Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
Mix_Chunk *chukatk,*ivenatk;
Mix_Music *music;
// SDL & Rendering Objects
SDL_Window* displayWindow;
GLuint fontTexture;
std::vector<GLuint> playerSpriteTexture, player2SpriteTexture;
GLuint groundTexture;
GLuint powerupTexture;
GLuint HALDUN, temple, fd, bf;

Matrix projectionMatrix;
Matrix viewMatrix;
Matrix modelMatrix;

ShaderProgram* program;
Ut ut; // drawText(), LoadTexture()

// GameLogic & Runtime Values
enum GameState { STATE_MAIN_MENU, STATE_GAME_LEVEL};
enum GameStage { FINAL_DESTINATION, BATTLEFIELD, TEMPLE };
int stage = FINAL_DESTINATION;
int state;
bool gameOver = false;
bool gameRunning = true;
float lastFrameTicks = 0.0f;
float elapsed;
bool dead = false;
float deathCounter = 0.0f;
#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6
#define ANAPEN 0.0001f
#define p1CD 0.7f
#define p2CD 1.0f

// Player Attributes. p1 is players[0]. p2 is players[1]
float playerSpeed = 3.0f;
bool p1controlsMoveLeft = false;
bool p1controlsMoveRight = false;
bool p1controlsJump = false;
bool p1firstJump = false;
bool p1secondJump = false;
float p1timeSinceLastJump = 0.0f;
int p1Health = 100;
bool p1NormalAttack;
bool p1StrongAttack;
bool p1UpAttack;

bool p2controlsMoveLeft = false;
bool p2controlsMoveRight = false;
bool p2controlsJump = false;
bool p2firstJump = false;
bool p2secondJump = false;
float p2timeSinceLastJump = 0.0f;
int p2Health = 100;
bool p2NormalAttack;
bool p2StrongAttack;
bool p2UpAttack;

// Game Object containers
std::vector<Entity> players;
std::vector<Entity> blocks;
std::vector<Entity> backgrounds;//BF, FD, Temple
Entity Hadimioglu;

// FUNCTIONS I CAN'T STICK ANYWHERE ELSE____________________________________________________________________________________________________________________________
void setUpStage(int& mapstage, std::vector<Entity>& blocks) {
	// Final Destination

	if (mapstage == FINAL_DESTINATION) {
		for (int i = 0; i < 50; i++) {
			blocks.push_back(Entity(-2.5f + (i)* 0.2f, 0.0f - (4 * 0.5f), 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, { groundTexture }, BLOCK));
		}
	}
	else if (mapstage == BATTLEFIELD) {
		for (int i = -5; i < 55; i++) {
			blocks.push_back(Entity(-2.5f + (i)* 0.2f, -2.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, { groundTexture }, BLOCK));
		}

		for (int i = 0; i < 50; i++) {
			blocks.push_back(Entity(-2.5f + (i)* 0.2f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, { groundTexture }, BLOCK));
			if (i == 10)
				i = 40;
		}

		for (int i = 20; i < 30; i++) {
			blocks.push_back(Entity(-2.5f + (i)* 0.2f, 2.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, { groundTexture }, BLOCK));
		}
	}
	else if (mapstage == TEMPLE) {
		for (int i = -5; i < 130; i++) {
			if (i == 55)
				i = 69;
			blocks.push_back(Entity(-2.5f + (i)* 0.2f, -1.8f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, { groundTexture }, BLOCK));
		}

		for (int i = 5; i < 45; i++) {
			blocks.push_back(Entity(-2.5f + (i)* 0.2f, 2.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, { groundTexture }, BLOCK));
		}

		for (int i = -5; i < 70; i++) {
			blocks.push_back(Entity(-2.5f + (i)* 0.2f, -5.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, { groundTexture }, BLOCK));
		}

		for (int i = 0; i < 18; i++) {
			blocks.push_back(Entity(11.3, -2.0f - (i * 0.2f), 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, { groundTexture }, BLOCK));
		}

		for (int i = 90; i < 110; i++) {
			blocks.push_back(Entity(-2.5f + (i)* 0.2f, 2.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, { groundTexture }, BLOCK));
			blocks.push_back(Entity(-2.5f + (i)* 0.2f, 4.8f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, { groundTexture }, BLOCK));
		}
	}
}

// RENDERING AND UPDATING CODE____________________________________________________________________________________________________________________________
void RenderMainMenu() {
	//draws text
	modelMatrix.identity();
	modelMatrix.Translate(-3.7f, 2.0f, 0.0f);
	program->setModelMatrix(modelMatrix);
	ut.DrawText(program, fontTexture, "IVEN VS CHUK", 0.2f, 0.0001f);

	modelMatrix.identity();
	modelMatrix.Translate(-0.5f, 2.0f, 0.0f);
	program->setModelMatrix(modelMatrix);
	if (stage == FINAL_DESTINATION)
		ut.DrawText(program, fontTexture, "MAP: FINAL DESTINATION", 0.2f, 0.0001f);
	else if (stage == BATTLEFIELD)
		ut.DrawText(program, fontTexture, "MAP: BATTLEFIELD", 0.2f, 0.0001f);
	else
		ut.DrawText(program, fontTexture, "MAP: TEMPLE", 0.2f, 0.0001f);
	
	modelMatrix.identity();
	modelMatrix.Translate(-0.5f, -1.25f, 0.0f);
	program->setModelMatrix(modelMatrix);
	Hadimioglu.draw(program);

	modelMatrix.identity();
	modelMatrix.Translate(-3.9f, -1.5f, 0.0f);
	program->setModelMatrix(modelMatrix);
	ut.DrawText(program, fontTexture, "USE ARROW/WASD KEYS TO MOVE & SELECT MAP", 0.2f, 0.0001f);

	modelMatrix.identity();
	modelMatrix.Translate(-2.2f, -1.75f, 0.0f);
	program->setModelMatrix(modelMatrix);
	ut.DrawText(program, fontTexture, "NUMPAD 1 / B TO ATTACK", 0.2f, 0.0001f);


	modelMatrix.identity();
	modelMatrix.Translate(-3.3f, -2.0f, 0.0f);
	program->setModelMatrix(modelMatrix);
	ut.DrawText(program, fontTexture, "PRESS SPACE TO START. ESC TO EXIT", 0.2f, 0.0001f);

}

void UpdateMainMenu(float elapsed) {
	// does nothing really. We just have static text to worry about here.
}

void RenderGameLevel() {
	backgrounds[stage].draw(program);
	players[1].draw(program);
	players[0].draw(program);
	for (size_t i = 0; i < blocks.size(); i++) {
		blocks[i].draw(program);
	}
	float averageViewX = (players[0].position[0] + players[1].position[0]) / 2;
	float averageViewY = (players[0].position[1] + players[1].position[1]) / 2;
	viewMatrix.identity();

	if (gameOver) {
		modelMatrix.identity();
		modelMatrix.Translate(averageViewX - 2.0f, averageViewY, 0.0f);
		program->setModelMatrix(modelMatrix);
		if (players[0].position[1] <= -19.0f || p1Health <= 0) {
			ut.DrawText(program, fontTexture, "IVEN WINS", 0.5f, 0.0001f);
		}
		else if (players[1].position[1] <= -19.0f || p2Health <= 0) {
			ut.DrawText(program, fontTexture, "CHUK WINS", 0.5f, 0.0001f);
		}
	}
	else {
		

		float distance = sqrt(pow(players[0].position[0] - players[1].position[0], 2) + pow(players[0].position[1] - players[1].position[1], 2));
		float scale = ut.map(distance, 0.0f, 18.0f, 1.0f, 0.05f);
		if (scale < 0.3f)
			scale = 0.3f;

		viewMatrix.Scale(scale, scale, 1.0f);
		viewMatrix.Translate(-averageViewX, -averageViewY, 0.0f);

		program->setViewMatrix(viewMatrix);
	}

	modelMatrix.identity();
	modelMatrix.Translate(players[0].position[0] - 0.25f, players[0].position[1] + 0.4f, 0.0f);
	program->setModelMatrix(modelMatrix);
	ut.DrawText(program, fontTexture, std::to_string(p1Health), 0.2f, 0.000001f);

	modelMatrix.identity();
	modelMatrix.Translate(players[1].position[0] - 0.25f, players[1].position[1] + 0.6f, 0.0f);
	program->setModelMatrix(modelMatrix);
	ut.DrawText(program, fontTexture, std::to_string(p2Health), 0.2f, 0.000001f);
}

void UpdateGameLevel(float elapsed) {
	for (int i = 0; i < 4; i++) {
		players[0].collided[i] = false;
		players[1].collided[i] = false;
	}
	float penetration;

	// Update all Y's first
	players[0].updateY(elapsed);
	players[1].updateY(elapsed);

	for (int k = 0; k < players.size(); k++) {
		for (size_t i = 0; i < blocks.size(); i++) {
			if (players[k].boundaries[1] < blocks[i].boundaries[0] &&
				players[k].boundaries[0] > blocks[i].boundaries[1] &&
				players[k].boundaries[2] < blocks[i].boundaries[3] &&
				players[k].boundaries[3] > blocks[i].boundaries[2])
			{
				float y_distance = fabs(players[k].position[1] - blocks[i].position[1]);
				float playerHeightHalf = 0.05f * players[k].size[1] * 2;
				float blockHeightHalf = 0.05f * blocks[i].size[1] * 2;
				penetration = fabs(y_distance - playerHeightHalf - blockHeightHalf);

				if (players[k].position[1] > blocks[i].position[1]) {
					players[k].position[1] += penetration + ANAPEN;
					players[k].boundaries[0] += penetration + ANAPEN;
					players[k].boundaries[1] += penetration + ANAPEN;
					players[k].collided[1] = true;
					players[k].inAir = false;
				}
				else {
					players[k].position[1] -= (penetration + ANAPEN);
					players[k].boundaries[0] -= (penetration + ANAPEN);
					players[k].boundaries[1] -= (penetration + ANAPEN);
					players[k].collided[0] = true;
				}
				players[k].speed[1] = 0.0f;
				break;
			}
		}
	}
	
	// Update all X's next
	players[0].updateX(elapsed);
	players[1].updateX(elapsed);
	for (int k = 0; k < players.size(); k++) {
		Entity player = players[k];
		for (size_t i = 0; i < blocks.size(); i++) {
			if (players[k].boundaries[1] < blocks[i].boundaries[0] &&
				players[k].boundaries[0] > blocks[i].boundaries[1] &&
				players[k].boundaries[2] < blocks[i].boundaries[3] &&
				players[k].boundaries[3] > blocks[i].boundaries[2])
			{
				float x_distance = fabs(players[k].position[0] - blocks[i].position[0]);
				float playerWidthHalf = 0.05f * players[k].size[0] * 2;
				float blockWidthHalf = 0.05f * blocks[i].size[0] * 2;
				penetration = fabs(x_distance - (playerWidthHalf + blockWidthHalf));

				if (players[k].position[0] > blocks[i].position[0]) {
					players[k].position[0] += penetration + ANAPEN;
					players[k].boundaries[2] += penetration + ANAPEN;
					players[k].boundaries[3] += penetration + ANAPEN;
					players[k].collided[3] = true;
				}
				else {
					players[k].position[0] -= (penetration + ANAPEN);
					players[k].boundaries[2] -= (penetration + ANAPEN);
					players[k].boundaries[3] -= (penetration + ANAPEN);
					players[k].collided[2] = true;
				}
				players[k].speed[0] = 0.0f;
				break;
			}
		}
	}

	players[0].speed[0] = 0.0f;
	players[1].speed[0] = 0.0f;
	
	// handle controls
	// Player 1
	if (p1controlsMoveLeft && (players[0].cooldown == 0 || players[0].inAir)){
		players[0].speed[0] = -playerSpeed*1.35;
		players[0].width = -1;
	}
	else if (p1controlsMoveRight && (players[0].cooldown == 0 || players[0].inAir)){
		players[0].speed[0] = playerSpeed*1.35;
		players[0].width = 1;
	}
	// Player 2
	if (p2controlsMoveLeft && (players[1].cooldown == 0 || players[1].inAir)){
		players[1].speed[0] = -playerSpeed;
		players[1].width = -1;
	}
	else if (p2controlsMoveRight && (players[1].cooldown == 0 || players[1].inAir)){
		players[1].speed[0] = playerSpeed;
		players[1].width = 1;
	}
	// Player 1 Attacks
	if (p1NormalAttack && players[0].cooldown == 0) {
		Mix_PlayChannel(1, chukatk, 0);
		if (!players[0].inAir) {
			players[0].cooldown = p1CD;
			float hitX = players[0].position[0] + (players[0].width * 0.5f);
			float hitY = players[0].position[1];
			float distance = sqrt(pow(hitX - players[1].position[0], 2) + pow(hitY - players[1].position[1], 2));

			if (distance < 0.7f) {
				players[1].speed[1] = 2.0f;
				p2Health -= 10;
				players[1].gettingWrecked = true;
				players[1].cooldown = 0.4;
			}
		}
		else {
			players[0].cooldown = p1CD;
			float hitX = players[0].position[0];
			float hitY = players[0].position[1] - 1.0f;
			float distance = sqrt(pow(hitX - players[1].position[0], 2) + pow(hitY - players[1].position[1], 2));

			if (distance < 0.7f) {
				players[1].speed[1] = 2.0f;
				p2Health -= 12;
				players[1].gettingWrecked = true;
				players[1].cooldown = 0.5;
			}
		}
		if (p2Health < 0)
			p2Health = 0;
	}
	// Player 2 Attacks
	if (p2NormalAttack && players[1].cooldown == 0) {
		players[1].cooldown = p2CD;
		Mix_PlayChannel(1, ivenatk, 0);
		if (!players[1].inAir) {
			players[1].cooldown = p2CD;
			float hitX = players[1].position[0] + (players[1].width * 0.5f);
			float hitY = players[1].position[1];
			float distance = sqrt(pow(hitX - players[0].position[0], 2) + pow(hitY - players[0].position[1], 2));

			if (distance < 0.5f) {
				players[0].speed[1] = 2.0f;
				p1Health -= 15;
				players[0].gettingWrecked = true;
				players[0].cooldown = 0.5;
			}
		}
		else {
			players[1].cooldown = p2CD;
			float hitX = players[1].position[0] + (players[1].width * 0.7f);
			float hitY = players[1].position[1] - 0.7f;
			float distance = sqrt(pow(hitX - players[0].position[0], 2) + pow(hitY - players[0].position[1], 2));

			if (distance < 0.7f) {
				players[0].speed[1] = 2.0f;
				p1Health -= 18;
				players[0].gettingWrecked = true;
				players[0].cooldown = 0.6;
			}
		}
		if (p1Health < 0)
			p1Health = 0;
	}
	// Player 1 JUMP
	if (players[0].collided[1]) {
		p1firstJump = false;
		p1secondJump = false;
	}
	if (p1controlsJump && players[0].cooldown == 0) {
		players[0].inAir = true;
		if (!p1firstJump && players[0].collided[1]) {
			players[0].speed[1] = 6.6f;
			p1timeSinceLastJump = 0.0f;
			p1firstJump = true;
			p1secondJump = false;
		}
		else if (p1firstJump && !p1secondJump && p1timeSinceLastJump > 0.4f ) {
			p1secondJump = true;
			players[0].speed[1] = 6.6f;
		}
	}
	if (!p1firstJump && !p1secondJump && p1controlsJump && !players[0].collided[1] && players[0].cooldown == 0) {
		p1secondJump = true;
		players[0].speed[1] = 6.6f;
	}
	// Player 2 JUMP
	if (players[1].collided[1]) {
		p2firstJump = false;
		p2secondJump = false;
	}
	if (p2controlsJump && players[1].cooldown == 0) {
		players[1].inAir = true;
		if (!p2firstJump && players[1].collided[1]) {
			players[1].speed[1] = 6.6f;
			p2timeSinceLastJump = 0.0f;
			p2firstJump = true;
			p2secondJump = false;
		}
		else if (p2firstJump && !p2secondJump && p2timeSinceLastJump > 0.4f) {
			p2secondJump = true;
			players[1].speed[1] = 6.6f;
		}
	}
	if (!p2firstJump && !p2secondJump && p2controlsJump && !players[1].collided[1] && players[1].cooldown == 0) {
		p2secondJump = true;
		players[1].speed[1] = 6.6f;
	}

	players[0].animate(elapsed);
	players[1].animate(elapsed);
	
	if (players[1].position[1] <= -19.0f || players[0].position[1] <= -19.0f || p1Health <= 0 || p2Health <= 0) {
		if (p1Health <= 0) {
			players[0].dead = true;
		}
		if (p2Health <= 0) {
			players[1].dead = true;
		}
		dead = true;
	}

	if (deathCounter >= 1.0f) {
		gameOver = true;
		gameRunning = false;
	}
}

void Render() {
	glClear(GL_COLOR_BUFFER_BIT);
	switch (state) {
	case STATE_MAIN_MENU:
		RenderMainMenu();
		break;
	case STATE_GAME_LEVEL:
		RenderGameLevel();
		break;
	}
	SDL_GL_SwapWindow(displayWindow);
}

void Update(float elapsed) {
	switch (state) {
	case STATE_MAIN_MENU:
		UpdateMainMenu(elapsed);
		break;
	case STATE_GAME_LEVEL:
		UpdateGameLevel(elapsed);
		break;
	}
}



// MAIN FUNCTION. SETUP____________________________________________________________________________________________________________________________
int main(int argc, char *argv[])
{
	srand(time(NULL));
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("GM Ivenchuk", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1600, 900, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif

	program = new ShaderProgram(RESOURCE_FOLDER"vertex_textured.glsl", RESOURCE_FOLDER"fragment_textured.glsl");
	SDL_Event event;
	bool done = false;

	projectionMatrix.setOrthoProjection(-4.0, 4.0, -2.25f, 2.25f, -1.0f, 1.0f);
	program->setModelMatrix(modelMatrix);
	program->setProjectionMatrix(projectionMatrix);
	program->setViewMatrix(viewMatrix);

	//Create GLUint textures
	fontTexture = ut.LoadTexture("font1.png");
	HALDUN = ut.LoadTexture("HaldunMode.png");
	Hadimioglu = Entity(0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 0, { HALDUN }, 21.5f, 21.5f, WIZARD);
	
	fd = ut.LoadTexture("FinalDestination.png");
	backgrounds.push_back(Entity(2.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 0, { fd }, 355.0f, 200.0f, WIZARD));
	bf = ut.LoadTexture("Battlefield.png");
	backgrounds.push_back(Entity(2.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 0, { bf }, 355.0f, 200.0f, WIZARD));
	temple = ut.LoadTexture("Temple.png");
	backgrounds.push_back(Entity(2.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0, 0, { temple }, 355.0f, 200.0f, WIZARD));
	
	playerSpriteTexture.push_back(ut.LoadTexture("ChukStanding1.png"));//Standing: 0-1
	playerSpriteTexture.push_back(ut.LoadTexture("ChukStanding2.png"));
	playerSpriteTexture.push_back(ut.LoadTexture("ChukJumping.png"));//Jumping: 2
	playerSpriteTexture.push_back(ut.LoadTexture("ChukRunning.png"));//Running: 3-5
	playerSpriteTexture.push_back(ut.LoadTexture("ChukRunning.png"));
	playerSpriteTexture.push_back(ut.LoadTexture("ChukRunning.png"));
	playerSpriteTexture.push_back(ut.LoadTexture("ChukGettingHit.png"));//Getting Hit: 6
	playerSpriteTexture.push_back(ut.LoadTexture("ChukGNormal1.png"));//Ground Attack: 7-8
	playerSpriteTexture.push_back(ut.LoadTexture("ChukGNormal2.png"));
	playerSpriteTexture.push_back(ut.LoadTexture("ChukANormal.png"));//Air Attack: 9
	playerSpriteTexture.push_back(ut.LoadTexture("ChukDeath1.png"));//Death: 10-11
	playerSpriteTexture.push_back(ut.LoadTexture("ChukDeath2.png"));

	player2SpriteTexture.push_back(ut.LoadTexture("IvenStanding1.png"));//Standing: 0-1
	player2SpriteTexture.push_back(ut.LoadTexture("IvenStanding2.png"));
	player2SpriteTexture.push_back(ut.LoadTexture("IvenJumping.png"));//Jumping: 2
	player2SpriteTexture.push_back(ut.LoadTexture("IvenRunning1.png"));//Running: 3-5
	player2SpriteTexture.push_back(ut.LoadTexture("IvenRunning2.png"));
	player2SpriteTexture.push_back(ut.LoadTexture("IvenRunning3.png"));
	player2SpriteTexture.push_back(ut.LoadTexture("IvenGettingHit.png"));//Getting Hit: 6
	player2SpriteTexture.push_back(ut.LoadTexture("IvenGNormal.png"));//Ground Attack: 7-8
	player2SpriteTexture.push_back(ut.LoadTexture("IvenGNormal.png"));
	player2SpriteTexture.push_back(ut.LoadTexture("IvenANormal.png"));//Air Attack: 9
	player2SpriteTexture.push_back(ut.LoadTexture("IvenDeath1.png"));//Death: 10-11
	player2SpriteTexture.push_back(ut.LoadTexture("IvenDeath2.png"));
	groundTexture = ut.LoadTexture("castleCenter.png");
	powerupTexture = ut.LoadTexture("cherry.png");

	//Sounds
	music = Mix_LoadMUS("VVVVVV Soundtrack 0616 Passion For Exploring.mp3");
	Mix_PlayMusic(music, -1);

	chukatk = Mix_LoadWAV("ChukHitsound.wav");
	ivenatk = Mix_LoadWAV("IvenHitsound.wav");
	//Mix_PlayChannel(1, chukatk, 0);
	//Mix_PlayChannel(2, ivenatk, 0);

	while (!done) {
		// Keyboard Controls
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE || event.key.keysym.scancode == SDL_SCANCODE_ESCAPE)
				done = true;
			switch (event.type) {
				case SDL_KEYDOWN:
					if (event.key.keysym.scancode == SDL_SCANCODE_SPACE) {
						//firing, starting the game
						if (gameOver == true) {
							gameOver = false;
							gameRunning = true;
							state = STATE_MAIN_MENU;
							ut.refresh(projectionMatrix, viewMatrix, modelMatrix, program);
						}
						else if (state == STATE_MAIN_MENU) {

							//Initialize entities
							players.clear();
							players.push_back(Entity(5.0f, -1.0f, 0.0f, -0.15f, 1.0f, 1.0f, 0, 0, playerSpriteTexture, 7.0f, 7.0f, PLAYER));//Chuk
							players.push_back(Entity(0.0f, -1.0f, 0.0f, -0.05f, 1.0f, 1.0f, 0, 0, player2SpriteTexture, 5.0f, 5.0f, PLAYER));//Iven
							players[0].width = -1;
							players[0].isStatic = false;
							players[0].acceleration[1] = -9.8f;
							players[1].isStatic = false;
							players[1].acceleration[1] = -9.8f;
							p1Health = 100;
							p2Health = 100;

							//Build map
							blocks.clear();
							setUpStage(stage, blocks);
							dead = false;
							deathCounter = 0.0f;
							state = STATE_GAME_LEVEL;
						}
					}
					if (event.key.keysym.scancode == SDL_SCANCODE_KP_1 || event.key.keysym.scancode == SDL_SCANCODE_I) {
						// Player 1 Neutral Attack
						p1NormalAttack = true;
					}
					if (event.key.keysym.scancode == SDL_SCANCODE_KP_2 || event.key.keysym.scancode == SDL_SCANCODE_O) {
						// Strong Attack
						p1StrongAttack = true;
					}
					if (event.key.keysym.scancode == SDL_SCANCODE_KP_3 || event.key.keysym.scancode == SDL_SCANCODE_P) {
						// Up Attack
						p1UpAttack = true;
					}
					if (event.key.keysym.scancode == SDL_SCANCODE_UP) {
						p1controlsJump = true;
					}
					if (event.key.keysym.scancode == SDL_SCANCODE_B) {
						// Player 2 Neutral Attack
						p2NormalAttack = true;
					}
					if (event.key.keysym.scancode == SDL_SCANCODE_N) {
						// Strong Attack
						p2StrongAttack = true;
					}
					if (event.key.keysym.scancode == SDL_SCANCODE_M) {
						// Up Attack
						p2UpAttack = true;
					}
					if (event.key.keysym.scancode == SDL_SCANCODE_W) {
						p2controlsJump = true;
					}
					if (state == STATE_MAIN_MENU) {
						if (event.key.keysym.scancode == SDL_SCANCODE_LEFT || event.key.keysym.scancode == SDL_SCANCODE_A) {
							if (stage > 0)
								stage--;
							else
								stage = TEMPLE;
						}
						if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT || event.key.keysym.scancode == SDL_SCANCODE_D) {
							if (stage < 2)
								stage++;
							else
								stage = FINAL_DESTINATION;
						}
					} else {
						if (event.key.keysym.scancode == SDL_SCANCODE_LEFT) {
							p1controlsMoveLeft = true;
						}
						else if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
							p1controlsMoveRight = true;
						}
						if (event.key.keysym.scancode == SDL_SCANCODE_A) {
							p2controlsMoveLeft = true;
						}
						else if (event.key.keysym.scancode == SDL_SCANCODE_D) {
							p2controlsMoveRight = true;
						}
					}
					break;
				case SDL_KEYUP:
					if (event.key.keysym.scancode == SDL_SCANCODE_KP_1 || event.key.keysym.scancode == SDL_SCANCODE_I) {
						// Neutral Attack
						p1NormalAttack = false;
					}
					if (event.key.keysym.scancode == SDL_SCANCODE_KP_2 || event.key.keysym.scancode == SDL_SCANCODE_O) {
						// Strong Attack
						p1StrongAttack = false;
					}
					if (event.key.keysym.scancode == SDL_SCANCODE_KP_3 || event.key.keysym.scancode == SDL_SCANCODE_P) {
						// Up Attack
						p1UpAttack = false;
					}
					if (event.key.keysym.scancode == SDL_SCANCODE_LEFT) {
						p1controlsMoveLeft = false;
					}
					if (event.key.keysym.scancode == SDL_SCANCODE_RIGHT) {
						p1controlsMoveRight = false;
					}
					if (event.key.keysym.scancode == SDL_SCANCODE_UP) {
						p1controlsJump = false;
					}
					if (event.key.keysym.scancode == SDL_SCANCODE_B) {
						// Player 2 Neutral Attack
						p2NormalAttack = false;
					}
					if (event.key.keysym.scancode == SDL_SCANCODE_N) {
						// Strong Attack
						p2StrongAttack = false;
					}
					if (event.key.keysym.scancode == SDL_SCANCODE_M) {
						// Up Attack
						p2UpAttack = false;
					}
					if (event.key.keysym.scancode == SDL_SCANCODE_A) {
						p2controlsMoveLeft = false;
					}
					if (event.key.keysym.scancode == SDL_SCANCODE_D) {
						p2controlsMoveRight = false;
					}
					if (event.key.keysym.scancode == SDL_SCANCODE_W) {
						p2controlsJump = false;
					}
					break;
			}
		}

		float ticks = (float)SDL_GetTicks() / 1000.0f;
		elapsed = ticks - lastFrameTicks;
		lastFrameTicks = ticks;

		if (gameRunning) {
			float fixedElapsed = elapsed;
			if (fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS) {
				fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
			}
			while (fixedElapsed >= FIXED_TIMESTEP) {
				fixedElapsed -= FIXED_TIMESTEP;
				Update(FIXED_TIMESTEP);
			}

			///
			p1timeSinceLastJump += fixedElapsed;
			p2timeSinceLastJump += fixedElapsed;
			if (players.size() >= 2){
				players[0].cooldown -= fixedElapsed;
				players[1].cooldown -= fixedElapsed;

				if (players[0].cooldown <= 0)
					players[0].cooldown = 0;

				if (players[1].cooldown <= 0)
					players[1].cooldown = 0;
			}
			if (dead)
				deathCounter += fixedElapsed;
			///

			Update(fixedElapsed);
			Render();
		}
	}

	Mix_FreeChunk(chukatk);
	Mix_FreeChunk(ivenatk);
	Mix_FreeMusic(music);

	SDL_Quit();
	return 0;
}
