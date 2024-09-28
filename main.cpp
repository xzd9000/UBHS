#include"main.h"

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
using std::sin;
using std::cos;

using namespace ubhs;

enum SpriteID
{
	SP_Test = 0,
	SP_Triangle = 1,
	SP_Ball = 2
};
enum ProjectileID
{
	PROJ_Triangle = 0,
	PROJ_Ball = 1,
	PROJ_TriangleEnemy = 2
};
enum CharacterID
{
	CH_RegEnemy = 0,
};

map<int, Projectile> projectiles;
map<int, Character> characters;

void LoadSprites();
void LoadProjectiles();
void LoadCharacters();
void AddEvents();

int main()
{
	wcout.imbue(locale("rus_rus.866"));	

	UBHS_Initialize();

	keymap.BindKey(KEY_Up,    GLFW_KEY_W);
	keymap.BindKey(KEY_Left,  GLFW_KEY_A);
	keymap.BindKey(KEY_Down,  GLFW_KEY_S);
	keymap.BindKey(KEY_Right, GLFW_KEY_D);

	keymap.BindKey(KEY_Shoot, GLFW_KEY_ENTER);

	LoadSprites();
	LoadProjectiles();
	LoadCharacters();
	AddEvents();

	timer.currentTime = glfwGetTime();
	while (!glfwWindowShouldClose(window)) MainLoop();

	glfwTerminate();
	return 0;
}

void LoadSprites()
{
	Texture testTexture, triangle, ball;
	unsigned char* testTextureData = stbi_load("testAxis.png", &testTexture.width, &testTexture.height, &testTexture.channellsNum, 0);
	unsigned char* triangleData = stbi_load("tri.png", &triangle.width, &triangle.height, &triangle.channellsNum, 0);
	unsigned char* ballData = stbi_load("ball.png", &ball.width, &ball.height, &ball.channellsNum, 0);
	testTexture.Generate(testTextureData);
	triangle.Generate(triangleData);
	ball.Generate(ballData);
	free(testTextureData);
	free(triangleData);
	free(ballData);

	shared_ptr<Sprite> testSprite = make_shared<Sprite>(SP_Test, spriteShaderProgram, &testTexture);
	shared_ptr<Sprite> triangleSprite = make_shared<Sprite>(SP_Triangle, spriteShaderProgram, &triangle);
	shared_ptr<Sprite> ballSprite = make_shared<Sprite>(SP_Ball, spriteShaderProgram, &ball);

	sprites.insert(pair<int, shared_ptr<Sprite>>(SP_Test, testSprite));
	sprites.insert(pair<int, shared_ptr<Sprite>>(SP_Triangle, triangleSprite));
	sprites.insert(pair<int, shared_ptr<Sprite>>(SP_Ball, ballSprite));
}

void LoadProjectiles()
{
	Projectile triangle = Projectile(
		sprites[SP_Triangle],
		unique_ptr<HitBox>(new HitBox(HitBox::T_AABB, vec2(1.4f, 1.f), CLL_PlayerBullet, CLL_Enemy)),
		vec2(0.f, 50.f), vec2(0.f, 1000.f),
		[](double, double, vec2) { return vec2(0.f); });
	triangle.scale = vec2(20.f);
	triangle.spriteColor = vec3(0.75f, 0.73f, 0.84f);

	Projectile triangleEnemy = Projectile(triangle);
	triangleEnemy.spriteColor = vec3(1.f, 0.f, 0.f);
	triangleEnemy.hitbox() -> collisionLayer = CLL_EnemyBullet;
	triangleEnemy.hitbox() -> collidesWithLayers = CLL_Player;

	Projectile ball = Projectile(sprites[SP_Ball], unique_ptr<HitBox>(new HitBox(HitBox::T_Circle, vec2(0.7f), CLL_EnemyBullet, CLL_Player)));
	ball.scale = vec2(25.f);

	projectiles.insert(pair<int, Projectile>(PROJ_Triangle, triangle));
	projectiles.insert(pair<int, Projectile>(PROJ_Ball, ball));
	projectiles.insert(pair<int, Projectile>(PROJ_TriangleEnemy, triangleEnemy));
}

void LoadCharacters()
{
	player = make_shared<Character>(sprites[SP_Triangle], unique_ptr<PlayerMovement>(new PlayerMovement(350.f, 0.45f, &keymap, &entities, windowHeigth, windowWidth, {
	projectiles.at(PROJ_Triangle) }
	)), unique_ptr<HitBox>(new HitBox(HitBox::T_Circle, vec2(0.25f), CLL_Player, CLL_Enemy | CLL_EnemyBullet)), 100.f);
	player -> checkCollisions = true;

	player -> collisionFunc = [](double t, double dt, Entity& owner, Entity& target)
	{
		owner.spriteColor = vec3(1.f, 0.f, 0.f);
		eventQueue.AddEvent(Event{ TL_Global, t + 5.0, [](double t_, double dt_) -> void
		{
			player -> spriteColor = vec3(1.f);
		} });
	};

	player -> position = vec2(0.f);
	player -> scale = vec2(50.f);

	entities.AddPlayer(player);


	Character regular = Character(sprites[SP_Triangle], nullptr, unique_ptr<HitBox>(new HitBox(HitBox::T_AABB, vec2(1.f), vec2(0.5f), CLL_Enemy, CLL_Player | CLL_PlayerBullet)), 30.f);

	regular.checkCollisions = true;
	regular.collisionFunc = [](double t, double dt, Entity& owner, Entity& target) -> void
	{
		if (target.hitbox() -> collisionLayer == CLL_PlayerBullet)
		{
			static_cast<Character&>(owner).AlterHP(-10.f);
			target.active = false;
		}
	};
	regular.rotation = 180.f;
	regular.scale = vec2(60.f);
	characters.insert(pair<int, Character>(CH_RegEnemy, regular));
}

void AddEvents()
{
	eventQueue.AddEvent({
		TL_Event,
		5.0,
		[](double t, double dt) -> void
		{ 
			Character* enemy = new Character(characters.at(CH_RegEnemy));
			shared_ptr<Entity> e = shared_ptr<Character>(enemy);

			enemy -> moveController = unique_ptr<SimpleMovement>(new SimpleMovement(vec2(800.f, 500.f), 200.f));
			enemy -> position = vec2(-50.f, 650.f);

			enemy -> attackController = unique_ptr<AttackController>(new AttackController(
				unique_ptr<AttackAction>(new CircleAttack(10.f, 300.f, &entities)),
				unique_ptr<AttackTime>  (new FixedPeriodAT(1))
			));
			enemy -> attackController -> projectiles.push_front(projectiles.at(PROJ_Ball));

			entities.AddEntity(e);
		}
	});
	eventQueue.AddEvent({
		TL_Event,
		9.0,
		[](double t, double dt) -> void
		{
			Character* enemy = new Character(characters.at(CH_RegEnemy));

			enemy -> position = vec2(750.f, 500.f);
			enemy -> moveController = unique_ptr<SimpleMovement>(new SimpleMovement(vec2(-100.f, 300.f), 200.f));

			enemy -> attackController = unique_ptr<AttackController>(new AttackController(
				unique_ptr<AttackAction>(new CircleAttack(10.f, 300.f, &entities)),
				unique_ptr<AttackTime>  (new FixedPeriodAT(1.0))
			));
			enemy -> attackController -> projectiles.push_front(projectiles.at(PROJ_Ball));

			entities.AddEntity(shared_ptr<Character>(enemy));
		}
	});
	eventQueue.AddEvent({
		TL_Event,
		13.0,
		[](double t, double dt) -> void
		{
			Character* enemy = new Character(characters.at(CH_RegEnemy));

			enemy -> position = vec2(-60.f, 600.f);
			enemy -> moveController = unique_ptr<SimpleMovement>(new SimpleMovement(vec2(1000.f, 600.f), 200.f));

			enemy -> attackController = unique_ptr<AttackController>(new AttackController(
				unique_ptr<AttackAction>(new TargetedAttack(player, 300.f, &entities)),
				unique_ptr<AttackTime>  (new FixedPeriodAT(0.33))
			));
			enemy -> attackController -> projectiles.push_front(projectiles.at(PROJ_Ball));

			entities.AddEntity(shared_ptr<Character>(enemy));
		}
	});
	eventQueue.AddEvent({
		TL_Event,
		17.0,
		[](double t, double dt) -> void
		{
			Character* enemy = new Character(characters.at(CH_RegEnemy));

			enemy -> position = vec2(760.f, 670.f);
			enemy -> moveController = unique_ptr<SimpleMovement>(new SimpleMovement(vec2(-1000.f, 670.f), 200.f));

			enemy -> attackController = unique_ptr<AttackController>(new AttackController(
				unique_ptr<AttackAction>(new TargetedAttack(player, 300.f, &entities)),
				unique_ptr<AttackTime>  (new FixedPeriodAT(0.33))
			));
			enemy -> attackController -> projectiles.push_front(projectiles.at(PROJ_Ball));

			entities.AddEntity(shared_ptr<Character>(enemy));
		}
	});
	eventQueue.AddEvent({
		TL_Event,
		21.0,
		[](double t, double dt) -> void
		{
			timer.eventTimeFrozen = true;

			Boss* boss = new Boss(characters.at(CH_RegEnemy), {
				BossPhase(9000.f, 1.25f, -1.f, false,
					unique_ptr<MovementController>(new SimpleMovement(vec2(350.f, 550.f), 200.f)),
					nullptr,
					nullptr
				),
				BossPhase(150.f, 10.f, -1.f, false,
					nullptr,
					unique_ptr<AttackAction>(new CircleAttack(11.f, 300.f, &entities)),
					unique_ptr<AttackTime>  (new FixedPeriodAT(0.6))
				),
				BossPhase(150.f, 10.f, 46.25f, true,
					unique_ptr<MovementController>(new RandomNPCMovement(vec4(150.f, 550.f, 450.f, 550.f), vec2(350.f, 550.f), 2.f, 350.f)),
					unique_ptr<AttackAction>(new TargetedAttack(player, 300.f, &entities)),
					unique_ptr<AttackTime>  (new FixedPeriodAT(0.33))
				)
			});

			boss -> attackController = unique_ptr<AttackController>(new AttackController());
			boss -> attackController -> projectiles.push_front(projectiles.at(PROJ_Ball));

			boss -> position = vec2(350.f, 750.f);
			boss -> scale = vec2(100.f);


			entities.AddEntity(shared_ptr<Boss>(boss));
		}
	});
	eventQueue.AddEvent({
		TL_Event,
		49.0,
		[](double t, double dt) -> void
		{
			Character* h1 = new Character(characters.at(CH_RegEnemy));

			h1 -> position = vec2(-60.f, 760.f);
			h1 -> moveController = unique_ptr<MovementController>(new SimpleMovement(vec2(100.f, 600.f), 400.f));
			h1 -> attackController = unique_ptr<AttackController>(new AttackController(
				unique_ptr<AttackAction>(new CircleAttack(12.f, 250.f, &entities)),
				unique_ptr<AttackTime>  (new FixedPeriodAT(0.9)),
				t + 3.0
			));
			h1 -> attackController -> projectiles.push_front(projectiles.at(PROJ_Ball));

			Character* h2 = new Character(*h1);

			h2 -> position = vec2(760.f, 760.f);
			h2 -> moveController = unique_ptr<MovementController>(new SimpleMovement(vec2(600.f, 600.f), 400.f));

			Character* l1 = new Character(characters.at(CH_RegEnemy));
			Character* l2 = new Character(characters.at(CH_RegEnemy));

			entities.AddEntity(shared_ptr<Character>(h1));
			entities.AddEntity(shared_ptr<Character>(h2));
		}
	});
	eventQueue.AddEvent({
		TL_Event,
		54.0,
		[](double t, double dt) -> void
		{
			Character* l1 = new Character(characters.at(CH_RegEnemy));

			l1 -> position = vec2(200.f, 760.f);
			l1 -> moveController = unique_ptr<MovementController>(new SimpleMovement(vec2(150.f, 500.f), 450.f));
			l1 -> attackController = unique_ptr<AttackController>(new AttackController(
				unique_ptr<AttackAction>(new SimpleAttack(&entities)),
				unique_ptr<AttackTime>  (new FixedPeriodAT(0.6)),
				t + 2.0
			));
			Projectile proj1 = Projectile(projectiles.at(PROJ_Triangle));

			proj1.hitbox() -> collidesWithLayers = CLL_Player;

			Projectile proj2 = Projectile(proj1);
			Projectile proj3 = Projectile(proj1);
			Projectile proj4 = Projectile(proj1);

			proj1.position = vec2(-0.4f, -0.6f) * 40.f;
			proj1.velocity = vec2(-0.4f, -0.6f) * 150.f;
			proj2.position = vec2( 0.4f, -0.6f) * 40.f;
			proj2.velocity = vec2( 0.4f, -0.6f) * 150.f;
			proj3.position = vec2(-0.1f, -0.8f) * 40.f;
			proj3.velocity = vec2(-0.2f, -0.8f) * 150.f;
			proj4.position = vec2( 0.2f, -0.8f) * 40.f;
			proj4.velocity = vec2( 0.2f, -0.8f) * 150.f;

			l1 -> attackController -> projectiles.push_front(proj1);
			l1 -> attackController -> projectiles.push_front(proj2);
			l1 -> attackController -> projectiles.push_front(proj3);
			l1 -> attackController -> projectiles.push_front(proj4);

			Character* l2 = new Character(*l1);

			l2 -> position = vec2(500.f, 760.f);
			l2 -> moveController = unique_ptr<MovementController>(new SimpleMovement(vec2(450.f, 500.f), 450.f));

			entities.AddEntity(shared_ptr<Character>(l1));
			entities.AddEntity(shared_ptr<Character>(l2));
		}
	});
	eventQueue.AddEvent({
		TL_Event,
		62.0,
		[](double t, double dt) -> void
		{
			entities.KillAllNPC();
			entities.DeactivateAllProjectiles();
		}
	});
	eventQueue.AddEvent({
		TL_Event,
		64.0,
		[](double t, double dt) -> void
		{
			timer.eventTimeFrozen = true;

			Projectile ballR(projectiles.at(PROJ_Ball)), ballB(ballR), tri(projectiles.at(PROJ_TriangleEnemy));

			ballR.spriteColor = vec3(1.f, 0.f, 0.f);
			ballB.spriteColor = vec3(0.f, 0.f, 1.f);

			float ph3ProjSpeed = 150.f;

			Projectile ph31 = Projectile(ballR);
			ph31.scale = vec3(100.f);

			Projectile ph32 = Projectile(ph31);
			Projectile ph33 = Projectile(ph31);
			Projectile ph34 = Projectile(ph31);
			Projectile ph35 = Projectile(ph31);

			ph31.velocity = vec2(-1.0f, 0.0f) * ph3ProjSpeed;
			ph32.velocity = vec2(-0.5f, 0.5f) * ph3ProjSpeed;
			ph33.velocity = vec2( 0.0f, 1.0f) * ph3ProjSpeed;
			ph34.velocity = vec2( 0.5f, 0.5f) * ph3ProjSpeed;
			ph35.velocity = vec2( 1.0f, 0.0f) * ph3ProjSpeed;

			Projectile ph3Tri1 = Projectile(tri);
			Projectile ph3Tri2 = Projectile(tri);
			Projectile ph3Tri3 = Projectile(tri);
			Projectile ph3Tri4 = Projectile(tri);

			ph3Tri1.velocity = vec2(-0.6, -0.4) * ph3ProjSpeed;
			ph3Tri2.velocity = vec2(-0.2, -0.8) * ph3ProjSpeed;
			ph3Tri3.velocity = vec2( 0.2, -0.8) * ph3ProjSpeed;
			ph3Tri4.velocity = vec2( 0.6, -0.4) * ph3ProjSpeed;

			Projectile ph331 = Projectile(ballB);
			Projectile ph332 = Projectile(ballR);
			Projectile ph333 = Projectile(ballB);
			Projectile ph334 = Projectile(ballB);
			Projectile ph335 = Projectile(ballR);
			Projectile ph336 = Projectile(ballB);

			ph331.velocity = vec2(-0.5f, -0.5f) * ph3ProjSpeed;
			ph332.velocity = vec2(-0.4f, -0.6f) * ph3ProjSpeed;
			ph333.velocity = vec2( 0.3f, -0.7f) * ph3ProjSpeed;
			ph334.velocity = vec2(-0.3f, -0.7f) * ph3ProjSpeed;
			ph335.velocity = vec2( 0.4f, -0.6f) * ph3ProjSpeed;
			ph336.velocity = vec2( 0.5f, -0.5f) * ph3ProjSpeed;

			ph333.acceleraionFunc = [](double t, double dt, vec2 pos) -> vec2
			{
				return vec2(0.f, -314.28);
			};
			ph334.acceleraionFunc = [](double t, double dt, vec2 pos) -> vec2
			{
				return vec2(0.f, -314.28);
			};

			Projectile phC1 = Projectile(ballR);
			Projectile phC2 = Projectile(ballR);

			phC1.velocity = vec2(-0.1f, -0.9f) * ph3ProjSpeed;
			phC2.velocity = vec2( 0.1f, -0.9f) * ph3ProjSpeed;

			Boss* boss = new Boss(characters.at(CH_RegEnemy), {
				BossPhase(9000.f, 1.25f, -1.f, false,
					unique_ptr<MovementController>(new SimpleMovement(vec2(350.f, 550.f), 200.f)),
					nullptr,
					nullptr
				),
				BossPhase(300.f, 25.f, -1.f, false,
					nullptr,
					unique_ptr<AttackAction>(new CircleAttack(15.f, 150.f, &entities, glm::pi<float>() / 30.f)),
					unique_ptr<AttackTime>(new FixedPeriodAT(0.5)),
					{ projectiles.at(PROJ_Ball) }
				),
				BossPhase(300.f, 10.f, -1.f, false,
					nullptr,
					unique_ptr<AttackAction>(new SimpleAttack(&entities)),
					unique_ptr<AttackTime>(new FixedPeriodAT(0.5)),
					{
						ph31, ph32, ph33, ph34, ph35,
						ph3Tri1, ph3Tri2, ph3Tri3, ph3Tri4,
						ph331, ph332, ph333, ph334, ph335, ph336,
						phC1, phC2
					}
				),
				BossPhase(450.f, 30.f, -1.f, true,
					nullptr,
					unique_ptr<AttackAction>(new RandomAttack(&entities, 200.f)),
					unique_ptr<AttackTime>(new FixedPeriodAT(0.2))
				)
			});

			boss -> attackController = unique_ptr<AttackController>(new AttackController());

			boss -> position = vec2(350.f, 750.f);
			boss -> scale = vec2(100.f);


			entities.AddEntity(shared_ptr<Boss>(boss));
		}
	});

}