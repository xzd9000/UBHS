#include "Texture.h"

Texture::Texture() : Texture(0, 0, 0, GL_REPEAT, GL_REPEAT, GL_NEAREST, GL_NEAREST) {  }
Texture::Texture(int width, int height, int channellsNum, int wrapS, int wrapT, int minFilter, int magFilter)
{
	Texture::width = width;
	Texture::height = height;
	Texture::channellsNum = channellsNum;
	Texture::wrapS = wrapS;
	Texture::wrapT = wrapT;
	Texture::minFilter = minFilter;
	Texture::magFilter = magFilter;
}
Texture::Texture(unsigned char* data, int width, int height, int channelsNum, int wrapS, int wrapT, int minFilter, int magFilter) : Texture(width, height, channelsNum, wrapS, wrapS, minFilter, magFilter)
{
	Generate(data, width, height);
}
void Texture::Generate(unsigned char* data, int width, int height)
{
	Texture::width = width;
	Texture::height = height;

	Generate(data);
}
void Texture::Generate(unsigned char* data)
{
	if (data)
	{
		glGenTextures(1, &ID);
		glBindTexture(GL_TEXTURE_2D, ID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapS);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);

		int channels = (channellsNum == 3) ? GL_RGB : GL_RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0, channels, width, height, 0, channels, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
}
