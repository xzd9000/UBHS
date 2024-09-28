#pragma once
#include"glad/glad.h"
struct Texture
{
	unsigned int ID;
	int width;
	int height;
	int channellsNum;
	int wrapS;
	int wrapT;
	int minFilter;
	int magFilter;

	Texture();
	Texture(int width, int height, int channellsNum, int wrapS, int wrapT, int minFilter, int magFilter);
	Texture(unsigned char* data, int width, int height, int channellsNum, int wrapS, int wrapT, int minFilter, int magFilter);

	void Generate(unsigned char* data, int width, int height);
	void Generate(unsigned char* data);
};

