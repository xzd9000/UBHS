#pragma once
#include<iostream>
#include"glm/glm.hpp"
#include"glm/gtc/matrix_transform.hpp"
#include"glm/gtc/type_ptr.hpp"
#include"Texture.h"

class Sprite
{
	private:
		static bool _initialized;
		static unsigned int _vertexArray;
		static float* _vertexData;
		static int _vertexSize;
		static unsigned int* _elements;
		static int _elementSize;

				 int _id;
		unsigned int _shaderProgram;
		Texture _texture;

	public:
		inline			int id() { return _id; }
		inline unsigned int shaderProgram() { return _shaderProgram; }
		inline unsigned int vertexArray() { return _vertexArray; }
		inline Texture texture() { return _texture; }

		void Draw(glm::mat4 projection, glm::vec3 color, glm::vec2 position, glm::vec2 scale, glm::vec2 pivot, float rotation);
		

		       Sprite(unsigned int id, unsigned int shaderProgram, Texture* texture);
		inline Sprite() { Sprite(0, 0, nullptr); }

		static void Initialize();
};

