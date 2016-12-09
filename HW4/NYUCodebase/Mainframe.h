
#include "Entity.h"

SDL_Window* displayWindow;

enum GameState { STATE_MAIN_MENU, STATE_GAME_LEVEL, STATE_END };

#define FIXED_TIMESTEP 0.0166666f
#define MAX_TIMESTEPS 6
float timeLeftOver = 0;

float lerp(float v0, float v1, float t) {
	return (1.0f - t)*v0 + t*v1;
}

GLuint LoadTexture(const char *image_path, GLenum format) {
	SDL_Surface *surface = IMG_Load(image_path);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SDL_FreeSurface(surface);

	return textureID;
}

void drText(int fontTexture, string text, float size, float spacing, float r, float g, float b, float a, float x, float y) {
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLoadIdentity();
	glTranslatef(x, y, 0.0f);
	float texture_size = 1.0 / 16.0f;
	vector<float> vertexData;
	vector<float> texCoordData;
	vector<float> colorData;
	for (unsigned int i = 0; i < text.size(); i++) {
		float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
		float texture_y = (float)(((int)text[i]) / 16) / 16.0f;

		colorData.insert(colorData.end(), { r, g, b, a, r, g, b, a, r, g, b, a, r, g, b, a });
		vertexData.insert(vertexData.end(), { ((size + spacing) * i) + (-0.5f * size), 0.5f * size, ((size + spacing) * i) +
			(-0.5f * size), -0.5f * size, ((size + spacing) * i) + (0.5f * size), -0.5f * size, ((size + spacing) * i) + (0.5f * size), 0.5f
			* size });
		texCoordData.insert(texCoordData.end(), { texture_x, texture_y, texture_x, texture_y + texture_size, texture_x +
			texture_size, texture_y + texture_size, texture_x + texture_size, texture_y });
	}
	glColorPointer(4, GL_FLOAT, 0, colorData.data());
	glEnableClientState(GL_COLOR_ARRAY);
	glVertexPointer(2, GL_FLOAT, 0, vertexData.data());
	glEnableClientState(GL_VERTEX_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, texCoordData.data());
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDrawArrays(GL_QUADS, 0, text.size() * 4);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
}

class Mainframe{
public:
	Mainframe();
	~Mainframe();

	void Init();
	bool UpdateAndRender();

	void Render();
	void Update();
private:
	int jumped;
	int enemyCount;
	void createEnemy();
	const Uint8* keys;
	bool done;
	float animation;
	GameState state;
	float lastFrameTicks;
	SDL_Window* displayWindow;
	GLuint font;

	Entity blocks[10];
	Entity enemy[25];
	int enemysize;
	float gravity;

	void reset();

	void RenderMainMenu();
	void RenderGameLevel();
	void RenderGameOver();

	void UpdateGameLevel();

	bool bumprightwall();
	bool bumpleftwall();
	float delay;
	Entity user;

	int score;
};

void Mainframe::reset(){
	score = 0;
	delay = 0.0f;
	enemyCount = 0;

	GLuint SheetSpriteTexture = LoadTexture("characters_1.png", GL_RGBA);
	GLuint blocktype1 = LoadTexture("castleMid.png", GL_RGBA);
	GLuint blocktype2 = LoadTexture("castleCenter.png", GL_RGBA);
	fill_n(blocks, 8, Entity(blocktype1, 0.0f, 0.0f, 1.0f, 1.0f, 0.13f));
	blocks[8] = Entity(blocktype2, 0.0f, 0.0f, 1.0f, 1.0f, 0.13f);
	blocks[9] = Entity(blocktype2, 0.0f, 0.0f, 1.0f, 1.0f, 0.13f);
	float blocksize = blocks[0].width*blocks[0].scale * 2.0f;
	float bottom = -2.0f + blocksize / 2.0f;
	float left = -2.66f + blocksize / 2.0f;
	float right = -left;

	for (int i = 0; i < 4; i++){
		blocks[i].width *= 8.0f;
	}

	for (int i = 0; i < 2; i++){
		blocks[i + 4].width *= 6.0f;
	}

	for (int i = 0; i < 2; i++){
		blocks[i + 6].width *= 11.0f;
	}

	for (int i = 0; i < 2; i++){
		blocks[i + 8].height *= 18.0f;
	}

	float leftplatform = left + 9.0f * blocksize / 2.0f;
	float rightplatform = -leftplatform;

	blocks[0].x = leftplatform;
	blocks[0].y = bottom;
	blocks[1].x = rightplatform;
	blocks[1].y = bottom;

	float tempheight = bottom + blocksize * 3.0f;

	blocks[6].x = 0.0f;
	blocks[6].y = tempheight;

	tempheight += blocksize * 3.0f;

	blocks[2].x = leftplatform;
	blocks[2].y = tempheight;
	blocks[3].x = rightplatform;
	blocks[3].y = tempheight;

	tempheight += blocksize * 3.0f;

	blocks[7].x = 0.0f;
	blocks[7].y = tempheight;

	tempheight += blocksize * 3.0f;

	leftplatform -= blocksize;
	rightplatform += blocksize;

	blocks[4].x = leftplatform;
	blocks[4].y = tempheight;
	blocks[5].x = rightplatform;
	blocks[5].y = tempheight;

	blocks[8].x = left;
	blocks[8].y = bottom + 17.0f * blocksize / 2.0f;
	blocks[9].x = right;
	blocks[9].y = bottom + 17.0f * blocksize / 2.0f;

	fill_n(enemy, 25, Entity(SheetSpriteTexture, 9.0f / 16.0f, 1.0f / 16.0f, 1.0f / 16.0f, 1.0f / 16.0f, 1.5f, true));
	for (int i = 0; i < 25; i++){
		enemy[i].u.push_back(10.0f / 16.0f);
		enemy[i].v.push_back(1.0f / 16.0f);
		enemy[i].u.push_back(11.0f / 16.0f);
		enemy[i].v.push_back(1.0f / 16.0f);
		enemy[i].u.push_back(9.0f / 16.0f);
		enemy[i].v.push_back(2.0f / 16.0f);
		enemy[i].u.push_back(10.0f / 16.0f);
		enemy[i].v.push_back(2.0f / 16.0f);
		enemy[i].u.push_back(11.0f / 16.0f);
		enemy[i].v.push_back(2.0f / 16.0f);
		enemy[i].u.push_back(0.0f);
		enemy[i].v.push_back(202.0f / 256.0f);
	}

	user = Entity(SheetSpriteTexture, 6.0f / 16.0f, 1.0f / 16.0f, 1.0f / 16.0f, 1.0f / 16.0f, 1.5f, true, true);
	user.u.push_back(7.0f / 16.0f);
	user.v.push_back(1.0f / 16.0f);
	user.u.push_back(8.0f / 16.0f);
	user.v.push_back(1.0f / 16.0f);
	user.u.push_back(6.0f / 16.0f);
	user.v.push_back(2.0f / 16.0f);
	user.u.push_back(7.0f / 16.0f);
	user.v.push_back(2.0f / 16.0f);
	user.u.push_back(8.0f / 16.0f);
	user.v.push_back(2.0f / 16.0f);
	user.x = 0.0f;
	user.y = -0.75f;
	user.v_x = 0.0f;
	user.v_y = 0.0f;
	user.f_x = 7.0f;
	user.f_y = 1.0f;
	user.a_x = 0.0f;
	user.elapsed = 0.0f;
	user.faceleft = false;

}

Mainframe::Mainframe() {
	Init();

	keys = SDL_GetKeyboardState(NULL);

	glViewport(0, 0, 800, 600);
	glMatrixMode(GL_PROJECTION);
	glOrtho(-2.66, 2.66, -2.0, 2.0, -2.0, 2.0);

	state = STATE_MAIN_MENU;

	jumped = 0;
	gravity = -9.81f;
	done = false;
	lastFrameTicks = 0.0f;
	enemysize = 12;
	enemyCount = 0;
	animation = 0.0f;

	font = LoadTexture("font1.png", GL_RGBA);

	glMatrixMode(GL_MODELVIEW);

}

void Mainframe::Init() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("My Game", SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

}

Mainframe::~Mainframe() {
	SDL_Quit();
}

void Mainframe::Render() {
	switch (state) {
	case STATE_MAIN_MENU:
		RenderMainMenu();
		break;
	case STATE_GAME_LEVEL:
		RenderGameLevel();
		break;
	case STATE_END:
		RenderGameOver();
		break;
	}
	SDL_GL_SwapWindow(displayWindow);
}

void Mainframe::RenderMainMenu(){
	glClearColor(0.4f, 0.5f, 0.9f, 0.0f);

	glClear(GL_COLOR_BUFFER_BIT);
	drText(font, "Brandario", 0.6f, -0.28f, 1.0f, 1.0f, 1.0f, 1.0f, -1.87f, 1.0f);
	drText(font, "Press SPACE to play", 0.4f, -0.2f, 1.0f, 1.0f, 1.0f, 1.0f, -1.87f, -1.3f);
	drText(font, "Arrow keys to move.", 0.2f, -0.1f, 1.0f, 1.0f, 1.0f, 1.0f, -1.8f, 0.4f);
	drText(font, "Space to jump, can jump again in midair", 0.2f, -0.1f, 1.0f, 1.0f, 1.0f, 1.0f, -1.8f, 0.2f);
	drText(font, "Jump onto enemies for points", 0.2f, -0.1f, 1.0f, 1.0f, 1.0f, 1.0f, -1.8f, 0.0f);
}

void Mainframe::createEnemy(){
	for (int i = 0; i < 25; i++){
		if (!enemy[i].alive && !enemy[i].coin){
			enemy[i].width = 1.0f / 16.0f;
			enemy[i].height = 1.0f / 16.0f;
			enemy[i].elapsed = 0.0f;
			enemy[i].alive = true;
			enemy[i].y = 2.5f;
			enemy[i].x = ((float)rand() / RAND_MAX - 0.5f) / 2.5f;

			if (rand() % 2 == 1){
				enemy[i].a_x = 0.35f;
				enemy[i].v_x = 0.65f;
			}
			else {
				enemy[i].a_x = -0.35f;
				enemy[i].v_x = -0.65f;
			}
			enemy[i].f_x = 0.35f;
			enemy[i].f_y = 0.35f;
			enemy[i].v_y = 0.0f;
			enemyCount++;
			enemy[i].coin = false;
			enemy[i].scale = 1.5f;
			break;
		}
	}
}

void Mainframe::RenderGameLevel(){
	glClearColor(0.4f, 0.5f, 0.9f, 0.0f);

	glClear(GL_COLOR_BUFFER_BIT);

	for (int i = 0; i < 25; i++){
		enemy[i].render();
	}

	for (int i = 0; i < 10; i++){
		blocks[i].render();
	}

	drText(font, "Score", 0.3f, -0.15f, 1.0f, 1.0f, 1.0f, 1.0f, 1.5f, 1.85f);

	drText(font, to_string(score), 0.3f, -0.15f, 1.0f, 1.0f, 1.0f, 1.0f, 1.5f, 1.6f);

	user.render();
}

void Mainframe::RenderGameOver(){
	glClearColor(0.4f, 0.5f, 0.9f, 0.0f);

	glClear(GL_COLOR_BUFFER_BIT);

	drText(font, "Score:", 0.2f, -0.1f, 1.0f, 1.0f, 1.0f, 1.0f, -0.8f, 0.45f);
	drText(font, to_string(score), 0.2f, -0.1f, 1.0f, 1.0f, 1.0f, 1.0f, -0.1f, 0.2f);

	drText(font, "Press SPACE to play again", 0.2f, -0.1f, 1.0f, 1.0f, 1.0f, 1.0f, -1.25f, -0.2f);
	drText(font, "Press ENTER to return", 0.2f, -0.1f, 1.0f, 1.0f, 1.0f, 1.0f, -1.15f, -0.4f);
}


void Mainframe::Update() {

	if (state == STATE_GAME_LEVEL){
		UpdateGameLevel();
	}

	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			done = true;
		}
		else if (event.type == SDL_KEYDOWN){
			if (state == STATE_GAME_LEVEL && event.key.keysym.scancode == SDL_SCANCODE_SPACE && !event.key.repeat){
				if (jumped == 0 && user.collidedBottom) {
					user.v_y = 3.7f;
					jumped = 1;
				}
				else if (jumped < 2) {
					user.v_y = 3.0f;
					jumped = 2;
				}
			}

			if (state == STATE_END){
				if (event.key.keysym.scancode == SDL_SCANCODE_RETURN){
					state = STATE_MAIN_MENU;
				}
				else if (event.key.keysym.scancode == SDL_SCANCODE_SPACE){
					state = STATE_GAME_LEVEL;
					reset();
				}
			}
			if (event.key.keysym.scancode == SDL_SCANCODE_SPACE && state == STATE_MAIN_MENU){
				state = STATE_GAME_LEVEL;
				reset();
			}


		}
	}
}

void Mainframe::UpdateGameLevel(){
	if (user.y < -2.0f){
		state = STATE_END;
	}
	float animation;

	if ((float)rand() / RAND_MAX > 0.985f) {
		if (enemyCount < 25){
			createEnemy();
		}
	}

	for (int i = 0; i < 25; i++){
		if (enemy[i].coin){
			enemy[i].resetcollision();

			enemy[i].v_y = lerp(enemy[i].v_y, 0.0f, FIXED_TIMESTEP * enemy[i].f_y);
			enemy[i].v_y += enemy[i].a_y * FIXED_TIMESTEP;
			enemy[i].v_y += gravity * FIXED_TIMESTEP;
			enemy[i].y += enemy[i].v_y * FIXED_TIMESTEP;
			for (int j = 0; j < 10; j++){
				if (enemy[i].collisiony(blocks[j])){
					enemy[i].v_y = 0.0f;
				}
			}

			enemy[i].elapsed += FIXED_TIMESTEP;
		}

		else if (enemy[i].alive){
			if (enemy[i].y < -2.5f){
				enemy[i].alive = false;
				enemyCount--;
				break;
			}

			enemy[i].elapsed += FIXED_TIMESTEP;

			animation = fmod(enemy[i].elapsed, FIXED_TIMESTEP*15.0f);
			enemy[i].faceleft = (enemy[i].v_x < 0);
			if (enemy[i].faceleft) {
				if (animation < FIXED_TIMESTEP*5.0f){
					enemy[i].frame = 0;
				}
				else if (animation < FIXED_TIMESTEP*10.0f){
					enemy[i].frame = 1;
				}
				else {
					enemy[i].frame = 2;
				}
			}
			else {
				if (animation < FIXED_TIMESTEP*5.0f){
					enemy[i].frame = 3;
				}
				else if (animation < FIXED_TIMESTEP*10.0f){
					enemy[i].frame = 4;
				}
				else {
					enemy[i].frame = 5;
				}
			}

			enemy[i].resetcollision();

			enemy[i].v_y = lerp(enemy[i].v_y, 0.0f, FIXED_TIMESTEP * enemy[i].f_y);
			enemy[i].v_y += enemy[i].a_y * FIXED_TIMESTEP;
			enemy[i].v_y += gravity * FIXED_TIMESTEP;
			enemy[i].y += enemy[i].v_y * FIXED_TIMESTEP;
			for (int j = 0; j < 10; j++){
				if (enemy[i].collisiony(blocks[j])){
					enemy[i].v_y = 0.0f;
				}
			}

			enemy[i].v_x = lerp(enemy[i].v_x, 0.0f, FIXED_TIMESTEP * enemy[i].f_x);
			enemy[i].v_x += enemy[i].a_x * FIXED_TIMESTEP;
			enemy[i].x += enemy[i].v_x * FIXED_TIMESTEP;
			for (int j = 0; j < 10; j++){
				if (enemy[i].collisionx(blocks[j])){
					if (enemy[i].collidedLeft){
						enemy[i].v_x = fabs(enemy[i].v_x);
						enemy[i].a_x = fabs(enemy[i].a_x);
					}
					else if (enemy[i].collidedRight){
						enemy[i].v_x = -fabs(enemy[i].v_x);
						enemy[i].a_x = -fabs(enemy[i].a_x);
					}
				}
			}
		}
	}



	user.elapsed += FIXED_TIMESTEP;

	animation = fmod(user.elapsed, FIXED_TIMESTEP*15.0f);

	if (user.faceleft) {
		if (fabs(user.v_x) < 0.2f){
			user.frame = 1;
		}
		else if (animation < FIXED_TIMESTEP*5.0f){
			user.frame = 0;
		}
		else if (animation < FIXED_TIMESTEP*10.0f){
			user.frame = 1;
		}
		else {
			user.frame = 2;
		}
	}
	else {
		if (fabs(user.v_x) < 0.2f){
			user.frame = 4;
		}
		else if (animation < FIXED_TIMESTEP*5.0f){
			user.frame = 3;
		}
		else if (animation < FIXED_TIMESTEP*10.0f){
			user.frame = 4;
		}
		else {
			user.frame = 5;
		}
	}


	user.resetcollision();

	user.v_y = lerp(user.v_y, 0.0f, FIXED_TIMESTEP * user.f_y);
	user.v_y += user.a_y * FIXED_TIMESTEP;
	user.v_y += gravity * FIXED_TIMESTEP;
	user.y += user.v_y * FIXED_TIMESTEP;
	for (int i = 0; i < 10; i++){
		if (user.collisiony(blocks[i])){
			user.v_y = 0.0f;
		}
	}

	if (user.collidedBottom){
		jumped = 0;
	}

	for (int i = 0; i < 25; i++){
		if (enemy[i].alive){
			if (user.collisiony(enemy[i])){
				if (user.collidedTop){
					state = STATE_END;
					break;
				}
				score += 100;
				jumped = 1;
				user.v_y = 2.0f;
				enemy[i].alive = false;
				enemy[i].v_y += 4.0f;
				enemy[i].elapsed = 10000.0f;
				enemy[i].coin = true;
				enemy[i].width = 40.0f / 256.0f;
				enemy[i].height = 40.0f / 256.0f;
				enemy[i].scale = 0.4f;
			}
		}

		if (enemy[i].coin){
			if (enemy[i].elapsed > 10000.6f && user.collision(enemy[i])){
				score += 200;
				enemy[i].coin = false;
				enemyCount--;
			}
		}
	}


	user.v_x = lerp(user.v_x, 0.0f, FIXED_TIMESTEP * user.f_x);
	user.v_x += user.a_x * FIXED_TIMESTEP;

	if (user.v_x > 1.5f){
		user.v_x = 1.5f;
	}
	else if (user.v_x < -1.5f){
		user.v_x = -1.5f;
	}

	user.x += user.v_x * FIXED_TIMESTEP;
	for (int j = 0; j < 10; j++){
		if (user.collisionx(blocks[j])){
			user.v_x = 0.0f;
			user.a_x = 0.0f;
		}
	}

	for (int i = 0; i < 25; i++){
		if (enemy[i].alive){
			if (user.collisionx(enemy[i])){
				state = STATE_END;
				break;
			}
		}
	}

	for (int i = 0; i < 25; i++){
		if (enemy[i].coin){
			if (enemy[i].elapsed > 10005.0f){
				enemy[i].coin = false;
			}
		}
	}


	if (keys[SDL_SCANCODE_LEFT]){
		user.v_x -= 18.0f * FIXED_TIMESTEP;
		user.faceleft = true;

	}
	if (keys[SDL_SCANCODE_RIGHT]){
		user.v_x += 18.0f * FIXED_TIMESTEP;
		user.faceleft = false;
	}
}

bool Mainframe::UpdateAndRender() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;

	float fixedElapsed = elapsed + timeLeftOver;
	if (fixedElapsed > FIXED_TIMESTEP * MAX_TIMESTEPS){
		fixedElapsed = FIXED_TIMESTEP * MAX_TIMESTEPS;
	}

	while (fixedElapsed >= FIXED_TIMESTEP){
		fixedElapsed -= FIXED_TIMESTEP;
		Update();
		Render();
		elapsed -= FIXED_TIMESTEP;
	}
	timeLeftOver = fixedElapsed;

	return done;
}
