#include"UBHS_Core.h"

using glm::mat4;
using glm::vec2;
using glm::vec3;
using glm::vec4;
using glm::ortho;

using std::vector;
using std::wcout;
using std::locale;
using std::unique_ptr;
using std::shared_ptr;
using std::make_shared;
using std::map;
using std::pair;
using std::forward_list;

GLFWwindow* ubhs::window;
int ubhs::success;
char ubhs::infoLog[512];

float ubhs::windowWidth = 700;
float ubhs::windowHeigth = 700;

mat4 ubhs::projection;

map<int, std::shared_ptr<Sprite>> ubhs::sprites;
EntityList ubhs::entities;
Keymap ubhs::keymap;

EventQueue ubhs::eventQueue;

unsigned int ubhs::spriteShaderProgram;

       Timer  ubhs::timer;
double Timer::accumulator = 0.0;
double Timer::currentTime = 0.0;
double Timer::timeWorld = 0.0;
double Timer::timeEvent = 0.0;
bool   Timer::eventTimeFrozen = false;
double Timer::dt = 1.0/60.0;
double Timer::newTime = 0.0;
double Timer::frameTime = 0.0;

shared_ptr<Character> ubhs::player;

#ifdef DEBUG_COLLISIONS
shared_ptr<Sprite> debugCircle;
shared_ptr<Sprite> debugBox;

enum DebugSprites
{
	SP_Debug_Circle = -1,
	SP_Debug_Box = -2,
};
#endif

void ubhs::CompileShader(unsigned int* shader, GLenum type, const char* shaderSource)
{
	*shader = glCreateShader(type);
	glShaderSource(*shader, 1, &shaderSource, 0);
	glCompileShader(*shader);

	glGetShaderiv(*shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(*shader, 512, nullptr, infoLog);
		wcout << L"Shaider compilation failed:\n" << infoLog << L"\n";
		system("pause");
	}
}
void ubhs::LinkShaders(unsigned int* shaderProgram, const char* vertexShaderSource, const char* fragmentShaderSource)
{
	unsigned int vertexShader;
	unsigned int fragmentShader;
	CompileShader(&vertexShader, GL_VERTEX_SHADER, vertexShaderSource);
	CompileShader(&fragmentShader, GL_FRAGMENT_SHADER, fragmentShaderSource);

	*shaderProgram = glCreateProgram();
	glAttachShader(*shaderProgram, vertexShader);
	glAttachShader(*shaderProgram, fragmentShader);
	glLinkProgram(*shaderProgram);
	glGetProgramiv(*shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(*shaderProgram, 512, nullptr, infoLog);
		wcout << L"Shaider link failed:\n" << infoLog << L"\n";
		system("pause");
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}


void ubhs::Resize(GLFWwindow* window, int width, int height)
{
	windowWidth = width;
	windowHeigth = height;
	glViewport(0, 0, width, height);
	UpdateProjection();
}


int ubhs::InitializeGLFW()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(700, 700, "Ky6", nullptr, nullptr);
	if (window == nullptr)
	{
		wcout << L"GLFW window creation failed\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		wcout << L"GLAD initialization failed\n";
		return -1;
	}

	glViewport(0, 0, windowHeigth, windowWidth);
	UpdateProjection();
	glfwSetFramebufferSizeCallback(window, Resize);

	return 0;
}

int ubhs::UBHS_Initialize()
{
	stbi_set_flip_vertically_on_load(true);

	if (InitializeGLFW() < 0)
	{
		std::system("pause");
		return -1;
	}
	LinkShaders(&spriteShaderProgram, spriteVertexShaderSource, spriteFragmentShaderSource);

	keymap.map.insert(pair<int, pair<int, bool>>((int)KEY_Shoot, pair<int, bool>(GLFW_KEY_Z,		  false)));
	keymap.map.insert(pair<int, pair<int, bool>>((int)KEY_Slow,  pair<int, bool>(GLFW_KEY_LEFT_SHIFT, false)));
	keymap.map.insert(pair<int, pair<int, bool>>((int)KEY_Up,	 pair<int, bool>(GLFW_KEY_UP,		  false)));
	keymap.map.insert(pair<int, pair<int, bool>>((int)KEY_Down,  pair<int, bool>(GLFW_KEY_DOWN,		  false)));
	keymap.map.insert(pair<int, pair<int, bool>>((int)KEY_Left,  pair<int, bool>(GLFW_KEY_LEFT,		  false)));
	keymap.map.insert(pair<int, pair<int, bool>>((int)KEY_Right, pair<int, bool>(GLFW_KEY_RIGHT,	  false)));

#ifdef DEBUG_COLLISIONS

	Texture debugBoxTexture, debugCircleTexture;
	unsigned char* boxData = stbi_load("debugBox.png", &debugBoxTexture.width, &debugBoxTexture.height, &debugBoxTexture.channellsNum, 0);
	unsigned char* circleData = stbi_load("debugCircle.png", &debugCircleTexture.width, &debugCircleTexture.height, &debugCircleTexture.channellsNum, 0);
	debugBoxTexture.Generate(boxData);
	debugCircleTexture.Generate(circleData);
	free(boxData);
	free(circleData);

	debugCircle = make_shared<Sprite>(SP_Debug_Circle, spriteShaderProgram, &debugCircleTexture);
	debugBox = make_shared<Sprite>(SP_Debug_Box, spriteShaderProgram, &debugBoxTexture);
	sprites.insert(pair<int, shared_ptr<Sprite>>(SP_Debug_Circle, debugCircle));
	sprites.insert(pair<int, shared_ptr<Sprite>>(SP_Debug_Box, debugBox));

#endif

	return 0;
}


void ubhs::MainLoop()
{
	timer.newTime = glfwGetTime();
	timer.frameTime = timer.newTime - timer.currentTime;
	timer.currentTime = timer.newTime;

	timer.accumulator += timer.frameTime;

	while (timer.accumulator >= timer.dt)
	{
		CheckInput(window);

		int pop = 0;
		for (auto e = eventQueue.q.begin(); e != eventQueue.q.end(); ++e)
		{
			if (e -> activationTime <= (e -> timeline == TL_Global ? timer.timeWorld : timer.timeEvent))
			{
				e -> action(timer.timeWorld, timer.dt);
				pop++;
			}
			else break;
		}
		for (int i = 0; i < pop; i++) eventQueue.q.pop_front();

		for (auto en = entities.list().begin(); en != entities.list().end(); ++en)
		{
			Entity& entity = *(*en);
			entity.ProcessMovement(timer.timeWorld, timer.dt);

			if (entity.position.x > windowWidth  + entity.scale.x * 1.2f || entity.position.x < -entity.scale.x * 1.2f ||
				entity.position.y > windowHeigth + entity.scale.y * 1.2f || entity.position.y < -entity.scale.y * 1.2f)
			{
				entity.active = false;
				continue;
			}

			if (entity.checkCollisions)
			{
				for (auto en2 = entities.list().begin(); en2 != entities.list().end(); ++en2)
				{
					Entity& entity2 = *(*en2);
					if (en != en2) if (entity2.hitbox() != nullptr) ProcessCollision(timer.timeWorld, timer.dt, entity, entity2, entity.hitbox() -> Collision(entity, entity2));
				}
			}
		}

		timer.accumulator -= timer.dt;
		timer.timeWorld += timer.dt;
		if (!timer.eventTimeFrozen) timer.timeEvent += timer.dt;
	}

	Render();

	glfwSwapBuffers(window);
	glfwPollEvents();
}

void ubhs::ProcessCollision(double t, double dt, Entity& owner, Entity& target, bool collision)
{
	if (collision)
	{
		owner.OnCollision(t, dt, owner, target);


#ifdef DEBUG_COLLISIONS

		target.spriteColor = vec3(1.f, 0.f, 0.f);

	}
	else target.spriteColor = vec3(1.f);
#else
	}
#endif
}
void ubhs::Render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	for (auto en = entities.list().begin(); en != entities.list().end(); ++en)
	{
		Entity& entity = *(*en);
		entity.Draw(projection);

#ifdef DEBUG_COLLISIONS
		if (entity.hitbox() != nullptr)
		{
			shared_ptr<Sprite> sprite;

			if (entity.hitbox() -> type == HitBox::T_Circle) sprite = debugCircle;
			else if (entity.hitbox() -> type == HitBox::T_AABB) sprite = debugBox;
			else sprite = sprites[0];

			entity.hitbox() -> DebugDrawHitbox(projection, sprite, entity);
		}
#endif
	}
}
