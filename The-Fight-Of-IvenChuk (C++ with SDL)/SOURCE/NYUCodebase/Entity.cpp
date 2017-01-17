#include "Entity.h"

Entity::Entity() {}

Entity::Entity(float x, float y, float spriteU, float spriteV, float spriteWidth, float spriteHeight, float dx, float dy, std::vector<GLuint> spriteTexture, Type newType) {
	position[0] = x;
	position[1] = y;
	speed[0] = dx;
	speed[1] = dy;
	acceleration[0] = 0;
	acceleration[1] = 0;
	entityMatrix.identity();
	entityMatrix.Translate(x, y, 0);
	size[0] = 1.0f;
	size[1] = 1.0f;
	boundaries[0] = y + 0.05f * size[1] * 2;
	boundaries[1] = y - 0.05f * size[1] * 2;
	boundaries[2] = x - 0.05f * size[0] * 2;
	boundaries[3] = x + 0.05f * size[0] * 2;

	u = spriteU;
	v = spriteV;
	width = spriteWidth;
	height = spriteHeight;
	texture = spriteTexture;
	currT = 0;
	type = newType;
}
Entity::Entity(float x, float y, float spriteU, float spriteV, float spriteWidth, float spriteHeight, float dx, float dy, std::vector<GLuint> spriteTexture, float sizeX, float sizeY, Type newType) {
	position[0] = x;
	position[1] = y;
	speed[0] = dx;
	speed[1] = dy;
	acceleration[0] = 0;
	acceleration[1] = 0;
	entityMatrix.identity();
	entityMatrix.Translate(x, y, 0);
	size[0] = sizeX;
	size[1] = sizeY;
	boundaries[0] = y + 0.05f * size[1] * 2;
	boundaries[1] = y - 0.05f * size[1] * 2;
	boundaries[2] = x - 0.05f * size[0] * 2;
	boundaries[3] = x + 0.05f * size[0] * 2;

	u = spriteU;
	v = spriteV;
	width = spriteWidth;
	height = spriteHeight;
	texture = spriteTexture;
	currT = 0;
	type = newType;
}

void Entity::draw(ShaderProgram* program) {
	entityMatrix.identity();
	entityMatrix.Translate(position[0], position[1], 0);
	program->setModelMatrix(entityMatrix);

	std::vector<float> vertexData;
	std::vector<float> texCoordData;
	float texture_x = u;
	float texture_y = v;
	vertexData.insert(vertexData.end(), {
		(-0.1f * size[0]), 0.1f * size[1],
		(-0.1f * size[0]), -0.1f * size[1],
		(0.1f * size[0]), 0.1f * size[1],
		(0.1f * size[0]), -0.1f * size[1],
		(0.1f * size[0]), 0.1f * size[1],
		(-0.1f * size[0]), -0.1f * size[1],
	});
	texCoordData.insert(texCoordData.end(), {
		texture_x, texture_y,
		texture_x, texture_y + height,
		texture_x + width, texture_y,
		texture_x + width, texture_y + height,
		texture_x + width, texture_y,
		texture_x, texture_y + height,
	});

	glUseProgram(program->programID);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program->texCoordAttribute);

	glBindTexture(GL_TEXTURE_2D, texture[currT]);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::update(float elapsed) {
	if (!isStatic) {

		speed[0] += acceleration[0];
		position[0] += speed[0] * elapsed;
		boundaries[2] += speed[0] * elapsed;
		boundaries[3] += speed[0] * elapsed;

		speed[1] += acceleration[1];
		position[1] += speed[1] * elapsed;
		boundaries[0] += speed[1] * elapsed;
		boundaries[1] += speed[1] * elapsed;
	}
}

void Entity::updateX(float elapsed) {
	if (!isStatic) {
		speed[0] += acceleration[0] * elapsed;
		position[0] += speed[0] * elapsed;
		boundaries[2] += speed[0] * elapsed;
		boundaries[3] += speed[0] * elapsed;
	}
}

void Entity::updateY(float elapsed) {
	if (!isStatic) {
		speed[1] += acceleration[1] * elapsed;
		position[1] += speed[1] * elapsed;
		boundaries[0] += speed[1] * elapsed;
		boundaries[1] += speed[1] * elapsed;
	}
}

void Entity::animate(float elapsed) {
	counter += elapsed;
	if (!dead){//Check Hit
		if (gettingWrecked){//Hit Animation
			currT = 6;
			if (cooldown<0.05f){
				gettingWrecked = false;
			}
		}
		else if (!inAir){//NOT in AIR
			if (cooldown >= 0.45f || attacking){
				attacking = true;
				currT = 7;
				if (cooldown < 0.5f){
					currT = 8;
				}
				if (cooldown < 0.05f){
					attacking = false;
				}
			}
			else if (speed[0] == 0){
				if (fmod(counter, 1) >= 0.5)
					currT = 0;
				else
					currT = 1;
			}
			else{
				if (fmod(counter, 0.9) >= 0.6)
					currT = 3;
				else if (fmod(counter, 0.9) >= 0.3)
					currT = 4;
				else
					currT = 5;
			}
		}
		else{//YES in AIR
			if (cooldown >= 0.45f || attacking){
				attacking = true;
				currT = 9;
				if (cooldown < 0.15f)
					attacking = false;
			}
			else{
				currT = 2;
			}
		}
	}
	else{
		if (counter > 1.5)
			counter = 0;
		currT = 10;
		if (counter > 0.5){
			currT = 11;
			counter = 0.6;
		}
	}
	if (currT >= texture.size())
		currT = 0;
	
}
