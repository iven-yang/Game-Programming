
#include "Entity.h"

enum GameState{ STATE_MAIN_MENU, STATE_GAME_LEVEL, STATE_END };

GLuint LoadTexture(const char *image_path, GLenum format) {
	SDL_Surface *surface = IMG_Load(image_path);

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, 4, surface->w, surface->h, 0, format, GL_UNSIGNED_BYTE, surface->pixels);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	SDL_FreeSurface(surface);

	return textureID;
}

void drawText(int fontTexture, string text, float size, float spacing, float x, float y){
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLoadIdentity();
	glTranslatef(x, y, 0.0f);

	float texture_size = 1.0 / 16.0f;
	vector<float> vertexData;
	vector<float> texCoordData;

	for (unsigned int i = 0; i < text.size(); i++) {
		float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
		float texture_y = (float)(((int)text[i]) / 16) / 16.0f;

		vertexData.insert(vertexData.end(), {
			((size+spacing)*i) + (-0.5f*size), 0.5f*size,
			((size+spacing)*i) + (-0.5f*size), -0.5f*size,
			((size+spacing)*i) + (0.5f*size), -0.5f*size,
			((size+spacing)*i) + (0.5f*size), 0.5f*size});

		texCoordData.insert(texCoordData.end(), {
			texture_x, texture_y,
			texture_x, texture_y + texture_size,
			texture_x + texture_size, texture_y + texture_size,
			texture_x + texture_size, texture_y});
	}

	glVertexPointer(2, GL_FLOAT, 0, vertexData.data());
	glEnableClientState(GL_VERTEX_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, texCoordData.data());
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glDrawArrays(GL_QUADS, 0, text.size() * 4);

	glDisableClientState(GL_VERTEX_ARRAY);
}

class Mainframe{
public:
	Mainframe();
	~Mainframe();

	void Setup();
	bool UpdateAndRender();

	void Render();
	void Update(float elapsed);

private:
	void reset();

	void RenderMainMenu();
	void RenderGameLevel();
	void RenderGameOver();

	void UpdateLevel(float elapsed);

	bool hitrightdetect();
	bool hitleftdetect();

	bool playerhit();
	bool playerhitdetect(Entity enemy);
	bool enemyhit(Entity enemy);
	bool shieldhit(Entity shield);
	bool shieldhitdetect(Entity& entity, Entity shield);
	bool invaded();
	
	const Uint8* keys;
	bool done;
	GameState state;
	SDL_Window* displayWindow;
	GLuint font;

	float lastFrameTicks, delay, playerhitdelay;
	Entity player, enemy1[12], enemy2[12], enemy3[12], enemy4[12], livesicon[3], shield[4];
	int score, lives, enemysize;
};

bool Mainframe::playerhitdetect(Entity enemy){
	if (((enemy.bullet.x + enemy.bullet.width*enemy.bullet.scale<player.x - player.width*player.scale) || (enemy.bullet.x - enemy.bullet.width*enemy.bullet.scale>player.x + player.width*player.scale) ||
		(enemy.bullet.y + enemy.bullet.height*enemy.bullet.scale<player.y - player.height*player.scale) || (enemy.bullet.y - enemy.bullet.height*enemy.bullet.scale>player.y + player.height*player.scale)) &&
		((enemy.bullet.x + enemy.bullet.width*enemy.bullet.scale<player.x - player.width*player.scale) || (enemy.bullet.x - enemy.bullet.width*enemy.bullet.scale>player.x + player.width*player.scale) ||
		(enemy.bullet.y + enemy.bullet.height*enemy.bullet.scale<player.y - player.height*player.scale) || (enemy.bullet.y - enemy.bullet.height*enemy.bullet.scale>player.y + player.height*player.scale))){
		return false;
	}
	return true;
}

bool Mainframe::shieldhitdetect(Entity& entity, Entity shield){
	if (shield.scale == 0){
		return false;
	}
	if (((entity.bullet.x + entity.bullet.width*entity.bullet.scale<shield.x - shield.width*shield.scale*2.4f) || (entity.bullet.x - entity.bullet.width*entity.bullet.scale>shield.x + shield.width*shield.scale*2.4f) ||
		(entity.bullet.y + entity.bullet.height*entity.bullet.scale<shield.y - shield.height*shield.scale) || (entity.bullet.y - entity.bullet.height*entity.bullet.scale>shield.y + shield.height*shield.scale)) &&
		((entity.bullet.x + entity.bullet.width*entity.bullet.scale<shield.x - shield.width*shield.scale*2.4f) || (entity.bullet.x - entity.bullet.width*entity.bullet.scale>shield.x + shield.width*shield.scale*2.4f) ||
		(entity.bullet.y + entity.bullet.height*entity.bullet.scale<shield.y - shield.height*shield.scale) || (entity.bullet.y - entity.bullet.height*entity.bullet.scale>shield.y + shield.height*shield.scale))){
		return false;
	}
	if (entity.y == player.y){
		entity.bullet.y = 2.0f;
		return true;
	}
	entity.bullet.y = -2.0f;
	return true;
}

bool Mainframe::shieldhit(Entity shield){
	bool hit = false;
	for (int i = 0; i < enemysize; i++){
		hit = hit || shieldhitdetect(enemy1[i], shield) || shieldhitdetect(enemy2[i], shield) ||
			shieldhitdetect(enemy3[i], shield) || shieldhitdetect(enemy4[i], shield);
		if (hit){
			return true;
		}
	}
	if (shieldhitdetect(player, shield)){
		return true;
	}
	return false;

}

bool Mainframe::enemyhit(Entity enemy){
	if (((player.bullet.x + player.bullet.width*player.bullet.scale<enemy.x - enemy.width*enemy.scale) || (player.bullet.x - player.bullet.width*player.bullet.scale>enemy.x + enemy.width*enemy.scale) ||
		(player.bullet.y + player.bullet.height*player.bullet.scale<enemy.y - enemy.height*enemy.scale) || (player.bullet.y - player.bullet.height*player.bullet.scale>enemy.y + enemy.height*enemy.scale)) &&
		((player.bullet.x + player.bullet.width*player.bullet.scale<enemy.x - enemy.width*enemy.scale) || (player.bullet.x - player.bullet.width*player.bullet.scale>enemy.x + enemy.width*enemy.scale) ||
		(player.bullet.y + player.bullet.height*player.bullet.scale<enemy.y - enemy.height*enemy.scale) || (player.bullet.y - player.bullet.height*player.bullet.scale>enemy.y + enemy.height*enemy.scale))){
		return false;
	}
	return true;
}

bool Mainframe::playerhit(){
	bool hit = false;
	for (int i = 0; i < enemysize; i++){
		hit = hit || (playerhitdetect(enemy1[i]) || playerhitdetect(enemy2[i]) || playerhitdetect(enemy3[i]) || playerhitdetect(enemy4[i]));
		if (hit) {
			return true;
		}
	}
	return false;
}

bool Mainframe::hitleftdetect(){
	for (int i = 0; i < enemysize; i++){
		if (enemy1[i].alive || enemy2[i].alive || enemy3[i].alive || enemy4[i].alive){
			if (min(min(enemy1[i].x, enemy2[i].x), min(enemy3[i].x, enemy4[i].x)) - enemy2[i].width*enemy1[i].scale < -1.33f){
				return true;
			}
		}
	}
	return false;
}

bool Mainframe::hitrightdetect(){
	for (int i = 0; i < enemysize; i++){
		if (enemy1[i].alive || enemy2[i].alive || enemy3[i].alive || enemy4[i].alive){
			if (max(max(enemy1[i].x, enemy2[i].x), max(enemy3[i].x, enemy4[i].x)) + enemy1[i].width*enemy1[i].scale > 1.33f){
				return true;
			}
		}
	}
	return false;
}

bool Mainframe::invaded(){
	for (int i = 0; i < enemysize; i++){
		if (enemy1[i].alive){
			if ((enemy1[i].y - enemy1[i].height*enemy1[i].scale) < -0.6f)
				return true;
		}
		if (enemy2[i].alive){
			if ((enemy2[i].y - enemy2[i].height*enemy2[i].scale) < -0.6f)
				return true;
		}
		if (enemy3[i].alive){
			if ((enemy3[i].y - enemy3[i].height*enemy3[i].scale) < -0.6f)
				return true;
		}
		if (enemy4[i].alive){
			if ((enemy4[i].y - enemy4[i].height*enemy4[i].scale) < -0.6f)
				return true;
		}
	}
	return false;
}

void Mainframe::reset(){
	score = 0;
	lives = 3;
	delay = 0.0f;

	GLuint SheetSpriteTexture = LoadTexture("sheet.png", GL_RGBA);
	fill_n(enemy1, enemysize, Entity(SheetSpriteTexture, 423.0f / 1024.0f, 728.0f / 1024.0f, 93.0f / 1024.0f, 84.0f / 1024.0f, 0.7f));
	fill_n(enemy2, enemysize, Entity(SheetSpriteTexture, 425.0f / 1024.0f, 468.0f / 1024.0f, 93.0f / 1024.0f, 84.0f / 1024.0f, 0.7f));
	fill_n(enemy3, enemysize, Entity(SheetSpriteTexture, 425.0f / 1024.0f, 552.0f / 1024.0f, 93.0f / 1024.0f, 84.0f / 1024.0f, 0.7f));
	fill_n(enemy4, enemysize, Entity(SheetSpriteTexture, 425.0f / 1024.0f, 384.0f / 1024.0f, 93.0f / 1024.0f, 84.0f / 1024.0f, 0.7f));

	float xtemp = -1.15f;
	float ytemp = 0.6f;

	for (int i = 0; i < enemysize; i++){
		enemy1[i].x = xtemp;
		enemy1[i].y = ytemp;
		enemy1[i].bullet = Bullet(SheetSpriteTexture, 856.0f / 1024.0f, 926.0f / 1024.0f, 9.0f / 1024.0f, 57.0f / 1024.0f, 0.6f, xtemp, ytemp, true);
		enemy2[i].x = xtemp;
		enemy2[i].y = ytemp - 0.18f;
		enemy2[i].bullet = Bullet(SheetSpriteTexture, 856.0f / 1024.0f, 926.0f / 1024.0f, 9.0f / 1024.0f, 57.0f / 1024.0f, 0.6f, xtemp, ytemp - 0.18f, true);
		enemy3[i].x = xtemp;
		enemy3[i].y = ytemp - 0.36f;
		enemy3[i].bullet = Bullet(SheetSpriteTexture, 856.0f / 1024.0f, 926.0f / 1024.0f, 9.0f / 1024.0f, 57.0f / 1024.0f, 0.6f, xtemp, ytemp - 0.36f, true);
		enemy4[i].x = xtemp;
		enemy4[i].y = ytemp - 0.54f;
		enemy4[i].bullet = Bullet(SheetSpriteTexture, 856.0f / 1024.0f, 926.0f / 1024.0f, 9.0f / 1024.0f, 57.0f / 1024.0f, 0.6f, xtemp, ytemp - 0.54f, true);
		xtemp += 0.18f;
	}

	player = Entity(SheetSpriteTexture, 112.0f / 1024.0f, 791.0f / 1024.0f, 112.0f / 1024.0f, 75.0 / 1024.0f, 0.5f);
	for (int i = 0; i < lives; i++){
		livesicon[i] = Entity(SheetSpriteTexture, 112.0f / 1024.0f, 791.0f / 1024.0f, 112.0f / 1024.0f, 75.0 / 1024.0f, 0.5f);
		livesicon[i].x = -0.79f + i*0.15f;
		livesicon[i].y = 0.8f;
	}
	player.x = 0.0f;
	player.y = -0.8f;
	player.bullet = Bullet(SheetSpriteTexture, 849.0f / 1024.0f, 480.0f / 1024.0f, 9.0f / 1024.0f, 57.0f / 1024.0f, 0.6f, player.x, player.y, false);

	xtemp = 0.3325f - 1.33f;
	for (int i = 0; i < 4; i++){
		shield[i] = Entity(SheetSpriteTexture, 0.0f / 1024.0f, 156.0f / 1024.0f, 144.0f / 1024.0f, 137.0 / 1024.0f, 0.6f);
		shield[i].x = xtemp + i*0.665f;
		shield[i].y = player.y + 0.15f;
	}
}

Mainframe::Mainframe() {
	Setup();

	keys = SDL_GetKeyboardState(NULL);

	glViewport(0, 0, 800, 600);
	glMatrixMode(GL_PROJECTION);
	glOrtho(-1.33, 1.33, -1.0, 1.0, -1.0, 1.0);

	state = STATE_MAIN_MENU;

	done = false;
	lastFrameTicks = 0.0f;
	enemysize = 12;

	font = LoadTexture("font1.png", GL_RGBA);

	glMatrixMode(GL_MODELVIEW);

}

void Mainframe::Setup() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Space Invaders", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
	#ifdef _WINDOWS
		glewInit();
	#endif
}

Mainframe::~Mainframe() {
	SDL_Quit();
}

void Mainframe::Render() {
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glClear(GL_COLOR_BUFFER_BIT);
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
	drawText(font, "SPACE INVADERS", 0.15f, -0.05f, -0.7f, 0.75f);
	drawText(font, "Use arrow keys to move and space to shoot", 0.1f, -0.05f, -1.0f, 0.0f);
	drawText(font, "Press SPACE to play", 0.15f, -0.05f, -0.9f, -0.6f);
}
void Mainframe::RenderGameLevel(){
	drawText(font, "Score:", 0.1f, -0.05f, 0.65f, 0.9f);
	drawText(font, "Lives:", 0.1f, -0.05f, -0.8f, 0.9f);
	for (int i = 0; i < lives; i++){
		livesicon[i].draw();
	}
	drawText(font, to_string(score), 0.1f, -0.05f, 0.65f, 0.8f);

	for (int i = 0; i < enemysize; i++){
		enemy1[i].bullet.draw();
		enemy1[i].render();
		enemy2[i].bullet.draw();
		enemy2[i].render();
		enemy3[i].bullet.draw();
		enemy3[i].render();
		enemy4[i].bullet.draw();
		enemy4[i].render();
	}

	player.render();
	player.bullet.draw();

	for (int i = 0; i < 4; i++){
		shield[i].draw(2.4f);
	}

}
void Mainframe::RenderGameOver(){
	drawText(font, "Score:", 0.15f, -0.05f, -0.7f, 0.75f);
	drawText(font, "Press SPACE", 0.15f, -0.05f, -1.0f, 0.0f);
	drawText(font, to_string(score), 0.1f, -0.05f, 0.1f, 0.75f);
}


void Mainframe::Update(float elapsed) {
	if (state == STATE_GAME_LEVEL)
		UpdateLevel(elapsed);

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE)
			done = true;
		else if (event.type == SDL_KEYDOWN){
			if (event.key.keysym.scancode == SDL_SCANCODE_SPACE){
				if (state == STATE_GAME_LEVEL){
					if (player.bullet.visible == false){
						player.bullet.x = player.x;
						player.bullet.y = player.y;
						player.bullet.visible = true;
					}
				}
				if (state == STATE_MAIN_MENU){
					state = STATE_GAME_LEVEL;
					reset();
				}
				if (state == STATE_END)
					state = STATE_MAIN_MENU;
			}
		}
	}
}

void Mainframe::UpdateLevel(float elapsed){
	if (lives == 0 || score == 7800 || invaded())
		state = STATE_END;
	else{
		for (int i = 0; i < 4; i++){
			if (shieldhit(shield[i])){
				if ((shield[i].scale) < 0.0f)
					shield[i].scale = 0.0f;
				else
					shield[i].scale -= 0.03f;
			}
		}

		delay -= elapsed;

		if (player.bullet.visible == true)
			player.bullet.y += elapsed * 2.0f;

		if (player.bullet.y > 1.2f)
			player.bullet.visible = false;

		if (playerhit()){
			for (int i = 0; i < enemysize; i++){
				enemy1[i].bullet.y = -2.0f;
				enemy1[i].bullet.x = enemy1[i].x;
				enemy1[i].shot = false;
				enemy2[i].bullet.y = -2.0f;
				enemy2[i].bullet.x = enemy2[i].x;
				enemy2[i].shot = false;
				enemy3[i].bullet.y = -2.0f;
				enemy3[i].bullet.x = enemy3[i].x;
				enemy3[i].shot = false;
				enemy4[i].bullet.y = -2.0f;
				enemy4[i].bullet.x = enemy4[i].x;
				enemy4[i].shot = false;
			}
			player.x = 0.0f;
			lives--;
		}

		if (delay < 0.0f) {
			for (int i = 0; i < enemysize; i++){	
				if (hitrightdetect() || hitleftdetect()){
					enemy1[i].vx = -(enemy1[i].vx)*1.1f;
					enemy2[i].vx = -(enemy2[i].vx)*1.1f;
					enemy3[i].vx = -(enemy3[i].vx)*1.1f;
					enemy4[i].vx = -(enemy4[i].vx)*1.1f;
					enemy1[i].y -= enemy1[i].vy;
					enemy2[i].y -= enemy2[i].vy;
					enemy3[i].y -= enemy3[i].vy;
					enemy4[i].y -= enemy4[i].vy;
					if (!enemy1[i].shot && enemy1[i].alive){
						enemy1[i].bullet.y = enemy1[i].y;
					}
					if (!enemy2[i].shot && enemy2[i].alive){
						enemy2[i].bullet.y = enemy2[i].y;
					}
					if (!enemy3[i].shot && enemy3[i].alive){
						enemy3[i].bullet.y = enemy3[i].y;
					}
					if (!enemy4[i].shot && enemy4[i].alive){
						enemy4[i].bullet.y = enemy4[i].y;
					}
				}
			}
			delay = 0.05f;
		}
	
		for (int i = 0; i < enemysize; i++){
			if (enemyhit(enemy1[i]) && enemy1[i].alive){
				score += 300;
				enemy1[i].x = 0.0f;
				enemy1[i].alive = false;
				if (!enemy1[i].shot){
					enemy1[i].bullet.visible = false;
				}
				player.bullet.y = 10.0f;
				player.bullet.visible = false;
				break;
			}
			else if (enemyhit(enemy2[i]) && enemy2[i].alive){
				score += 200;
				enemy2[i].x = 0.0f;
				enemy2[i].alive = false;
				if (!enemy2[i].shot){
					enemy2[i].bullet.visible = false;
				}
				player.bullet.y = 10.0f;
				player.bullet.visible = false;
				break;
			}
			else if (enemyhit(enemy3[i]) && enemy3[i].alive){
				score += 100;
				enemy3[i].x = 0.0f;
				enemy3[i].alive = false;
				if (!enemy3[i].shot){
					enemy3[i].bullet.visible = false;
				}
				player.bullet.y = 10.0f;
				player.bullet.visible = false;
				break;
			}
			else if (enemyhit(enemy4[i]) && enemy4[i].alive){
				score += 50;
				enemy4[i].x = 0.0f;
				enemy4[i].alive = false;
				if (!enemy4[i].shot){
					enemy4[i].bullet.visible = false;
				}
				player.bullet.y = 10.0f;
				player.bullet.visible = false;
				break;
			}
		}
	
		for (int i = 0; i < enemysize; i++){
			if (!enemy1[i].alive && enemy1[i].bullet.y < -2.0f){}
			else {
				if (enemy1[i].shot){
					enemy1[i].bullet.y -= enemy1[i].bullet.speed*elapsed;
				}
				else if (enemy1[i].alive) {
					enemy1[i].bullet.x += enemy1[i].vx*elapsed;
				}
				else if (enemy1[i].alive && !enemy1[i].shot){
					enemy1[i].bullet.y = -2.0f;
				}
			}
	
			if (!enemy2[i].alive && enemy2[i].bullet.y < -2.0f){}
			else {
				if (enemy2[i].shot){
					enemy2[i].bullet.y -= enemy2[i].bullet.speed*elapsed;
				}
				else if (enemy2[i].alive){
					enemy2[i].bullet.x += enemy2[i].vx*elapsed;
				}
				else if (enemy2[i].alive && !enemy2[i].shot){
					enemy2[i].bullet.y = -2.0f;
				}
			}
	
			if (!enemy3[i].alive && enemy3[i].bullet.y < -2.0f){}
			else {
				if (enemy3[i].shot){
					enemy3[i].bullet.y -= enemy3[i].bullet.speed*elapsed;
				}
				else if (enemy3[i].alive){
					enemy3[i].bullet.x += enemy3[i].vx*elapsed;
				}
				else if (enemy3[i].alive && !enemy3[i].shot){
					enemy3[i].bullet.y = -2.0f;
				}
			}
	
			if (!enemy1[i].alive && enemy1[i].bullet.y < -2.0f){}
			else {
				if (enemy4[i].shot){
					enemy4[i].bullet.y -= enemy4[i].bullet.speed*elapsed;
				}
				else if (enemy4[i].alive){
					enemy4[i].bullet.x += enemy4[i].vx*elapsed;
				}
				else if (enemy4[i].alive && !enemy4[i].shot){
					enemy4[i].bullet.y = -2.0f;
				}
			}
			if (enemy1[i].alive){
				enemy1[i].pauseoverlay -= elapsed;
				enemy1[i].x += enemy1[i].vx*elapsed;
				if (enemy1[i].pauseoverlay < -900.0f) {
					enemy1[i].pauseoverlay = rand() % 12 + 1.0f;
				}
				else if (enemy1[i].pauseoverlay < 0.0f){
					enemy1[i].shot = true;
					enemy1[i].pauseoverlay = rand() % 12 + 4.0f;
				}
				if (enemy1[i].bullet.y < -1.5f) {
					enemy1[i].bullet.x = enemy1[i].x;
					enemy1[i].bullet.y = enemy1[i].y;
					enemy1[i].shot = false;
				}
			}
			if (enemy2[i].alive){
				enemy2[i].pauseoverlay -= elapsed;
				enemy2[i].x += enemy2[i].vx*elapsed;
				if (enemy2[i].pauseoverlay < -900.0f) {
					enemy2[i].pauseoverlay = rand() % 12 + 1.0f;
				}
				else if (enemy2[i].pauseoverlay < 0.0f){
					enemy2[i].shot = true;
					enemy2[i].pauseoverlay = rand() % 12 + 4.0f;
				}
				if (enemy2[i].bullet.y < -1.5f) {
					enemy2[i].bullet.x = enemy2[i].x;
					enemy2[i].bullet.y = enemy2[i].y;
					enemy2[i].shot = false;
				}
			}
			if (enemy3[i].alive){
				enemy3[i].pauseoverlay -= elapsed;
				enemy3[i].x += enemy3[i].vx*elapsed;
				if (enemy3[i].pauseoverlay < -900.0f) {
					enemy3[i].pauseoverlay = rand() % 12 + 1.0f;
				}
				else if (enemy3[i].pauseoverlay < 0.0f){
					enemy3[i].shot = true;
					enemy3[i].pauseoverlay = rand() % 12 + 4.0f;
				}
				if (enemy3[i].bullet.y < -1.5f) {
					enemy3[i].bullet.x = enemy3[i].x;
					enemy3[i].bullet.y = enemy3[i].y;
					enemy3[i].shot = false;
				}
			}
			if (enemy4[i].alive){
				enemy4[i].pauseoverlay -= elapsed;
	
				enemy4[i].x += enemy4[i].vx*elapsed;
				if (enemy4[i].pauseoverlay < -900.0f) {
					enemy4[i].pauseoverlay = rand() % 12 + 1.0f;
				}
				else if (enemy4[i].pauseoverlay < 0.0f){
					enemy4[i].shot = true;
					enemy4[i].pauseoverlay = rand() % 12 + 4.0f;
				}
				if (enemy4[i].bullet.y < -1.5f) {
					enemy4[i].bullet.x = enemy4[i].x;
					enemy4[i].bullet.y = enemy4[i].y;
					enemy4[i].shot = false;
				}
			}
		}
		if (keys[SDL_SCANCODE_LEFT]){
			if (player.x - player.width*player.scale > -1.33){
				player.x -= 0.75f * elapsed;
			}
		}
		else if (keys[SDL_SCANCODE_RIGHT]){
			if (player.x + player.width*player.scale < 1.33){
				player.x += 0.75f * elapsed;
			}
		}
	}
}

bool Mainframe::UpdateAndRender() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float elapsed = ticks - lastFrameTicks;
	lastFrameTicks = ticks;
	Update(elapsed);
	Render();
	return done;
}

SDL_Window* displayWindow;