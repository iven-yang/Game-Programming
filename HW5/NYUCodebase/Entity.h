
#include "Vectors.h"

Matrix projectionMatrix;
Matrix viewMatrix;
Matrix modelMatrix;

ShaderProgram* program;

class Entity {
public:
	Entity() {}

	Entity(float x, float y, float u, float v, float width, float height, float dx, float dy, GLuint texture, float xH, float yH, float newAngle):x(x),y(y),u(u),v(v),width(width),height(height),vx(dx),vy(dy),ax(0),ay(0),texture(texture),xH(xH),yH(yH){
		entityMatrix.identity();
		entityMatrix.Translate(x, y, 0);
		boundaries[0] = y + 0.1f * yH;
		boundaries[1] = y - 0.1f * yH;
		boundaries[2] = x - 0.1f * xH;
		boundaries[3] = x + 0.1f * xH;

		angle = newAngle;
		Vectors v1(boundaries[2], boundaries[0]); v1.rotate(x, y, angle);
		Vectors v2(boundaries[3], boundaries[0]); v2.rotate(x, y, angle);
		Vectors v3(boundaries[3], boundaries[1]); v3.rotate(x, y, angle);
		Vectors v4(boundaries[2], boundaries[1]); v4.rotate(x, y, angle);
		corners.push_back(v1);
		corners.push_back(v2);
		corners.push_back(v3);
		corners.push_back(v4);
	}

	void draw();
	void update(float elapsed);
	void collide();

	Matrix entityMatrix;
	float x, y, u, v, width, height, vx, vy, ax, ay, xH, yH;
	float boundaries[4];
	vector<Vectors> corners;
	float angle;

	GLuint texture;
	bool collided[4];
};

void Entity::draw() {
	entityMatrix.identity();
	entityMatrix.Translate(x, y, 0);
	entityMatrix.Rotate(angle);
	program->setModelMatrix(entityMatrix);

	vector<float> vertexData, texCoordData;
	vertexData.insert(vertexData.end(), {
		(-0.1f*xH), 0.1f*yH,
		(-0.1f*xH), -0.1f*yH,
		(0.1f*xH), 0.1f*yH,
		(0.1f*xH), -0.1f*yH,
		(0.1f*xH), 0.1f*yH,
		(-0.1f*xH), -0.1f*yH,
	});
	texCoordData.insert(texCoordData.end(),{u, v, u, v + height, u + width, v, u + width, v + height, u + width, v, u, v + height,});

	glUseProgram(program->programID);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertexData.data());
	glEnableVertexAttribArray(program->positionAttribute);
	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoordData.data());
	glEnableVertexAttribArray(program->texCoordAttribute);

	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::update(float elapsed) {
	vx += ax; vy += ay;
	x += vx * elapsed; y += vy * elapsed;
	for (size_t i = 0; i < corners.size(); ++i){
		corners[i].x += vx * elapsed;
		corners[i].y += vy * elapsed;
	}
	boundaries[0] += vy * elapsed;
	boundaries[1] += vy * elapsed;
	boundaries[2] += vx * elapsed;
	boundaries[3] += vx * elapsed;
}

void Entity::collide(){
	vx = -vx;
	vy = -vy;
	ax = ax;
	ay = ay;
}