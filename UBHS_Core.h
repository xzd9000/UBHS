#pragma once

#include"glad/glad.h"
#include"GLFW/glfw3.h"
#include"glm/glm.hpp"
#include"glm/gtc/matrix_transform.hpp"
#include"glm/gtc/type_ptr.hpp"
#include"stb_image.h"
#include"Sprite.h"
#include"Entity.h"
#include"Projectile.h"
#include"EntityList.h"
#include"PlayerMovement.h"
#include"Character.h"
#include"UBHS_Definitions.h"
#include"HitBox.h"
#include"Event.h"
#include"EventQueue.h"
#include"SimpleMovement.h"
#include"RandomNPCMovement.h"
#include"StaticEntity.h"
#include"Timer.h"
#include"Boss.h"

namespace ubhs 
{
	extern GLFWwindow* window;
	extern int success;
	extern char infoLog[512];

	extern float windowWidth;
	extern float windowHeigth;

	extern glm::mat4 projection;

	extern std::map<int, std::shared_ptr<Sprite>> sprites;
	extern EntityList entities;
	extern Keymap keymap;

	extern EventQueue eventQueue;

	extern unsigned int spriteShaderProgram;

	extern Timer timer;

	extern std::shared_ptr<Character> player;

	extern const char* spriteVertexShaderSource;
	extern const char* spriteFragmentShaderSource;

	inline void UpdateProjection() { projection = glm::ortho<float>(0.f, windowWidth, 0.f, windowHeigth, -1.f, 1.f); }
	inline void CheckInput(GLFWwindow* window) { for (auto& key : keymap.map) { key.second.second = glfwGetKey(window, key.second.first); } }

	extern void CompileShader(unsigned int* shader, GLenum type, const char* shaderSource);
	extern void LinkShaders(unsigned int* shaderProgram, const char* vertexShaderSource, const char* fragmentShaderSource);

	extern int InitializeGLFW();
	extern void Resize(GLFWwindow* window, int width, int height);

	extern int UBHS_Initialize();
	extern void ProcessCollision(double t, double dt, Entity& owner, Entity& target, bool collision);
	extern void Render();
	extern void MainLoop();
}