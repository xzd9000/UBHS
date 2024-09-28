#include"Sprite.h"

using glm::mat4;
using glm::vec3;
using glm::vec2;
using glm::radians;
using glm::translate;
using glm::rotate;
using glm::scale;
using glm::value_ptr;

using std::cout;

bool Sprite::_initialized = false;
unsigned int Sprite::_vertexArray;
float* Sprite::_vertexData;
int Sprite::_vertexSize;
unsigned int* Sprite::_elements;
int Sprite::_elementSize;

void Sprite::Initialize()
{
	glGenVertexArrays(1, &_vertexArray);
	float vertexData [] = 
	{
			0.0f, 1.0f, 0.0f, 1.0f,
			1.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 0.0f,
			1.0f, 1.0f, 1.0f, 1.0f,
	};
	_vertexData = vertexData;
	_vertexSize = sizeof(vertexData);
	_elements = new unsigned int []
	{
		0, 1, 2,
		0, 3, 1
	};
	_elementSize = 6 * sizeof(unsigned);

	unsigned int vertexBuffer, elementBuffer;
	glGenBuffers(1, &vertexBuffer);
	glGenBuffers(1, &elementBuffer);
	glBindVertexArray(_vertexArray);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, _vertexSize, _vertexData, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _elementSize, _elements, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
}

Sprite::Sprite(unsigned int id, unsigned int shaderProgram, Texture* texture = 0)
{
	if (_initialized == false) Initialize();

	if (texture != 0) _texture = *texture;

	_shaderProgram = shaderProgram;
}


void Sprite::Draw(mat4 projection, vec3 color, vec2 position, vec2 scale, vec2 pivot, float rotation)
{
	glBindVertexArray(_vertexArray);

	mat4 transform = mat4(1.f);

	transform = translate(transform, vec3(position, 0.f));

	transform = rotate(transform, radians(rotation), vec3(0.f, 0.f, 1.f));
	transform = glm::scale(transform, vec3(scale, 1.f));

	transform = translate(transform, vec3(-pivot.x, -pivot.y, 0.f));


	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture.ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glUseProgram(_shaderProgram);
	glUniformMatrix4fv(glGetUniformLocation(_shaderProgram, "transform"), 1, GL_FALSE, value_ptr(transform));
	glUniformMatrix4fv(glGetUniformLocation(_shaderProgram, "projection"), 1, GL_FALSE, value_ptr(projection));
	glUniform3fv(glGetUniformLocation(_shaderProgram, "spriteColor"), 1, value_ptr(color));

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}
