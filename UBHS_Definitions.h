#pragma once
#include<map>

class Entity;
class Character;
class AttackController;
class EntityList;

using AccelerationFunc = glm::vec2(*)(double t, double dt, glm::vec2 pos);
using FuncTDT = void(*)(double t, double dt);
using CollisionFunc = void(*)(double t, double dt, Entity& owner, Entity& target);
using AttackFunc = void(*)(Character& owner, AttackController& this_, double t, double dt);

struct Keymap
{
	std::map<int, std::pair<int, bool>> map;

	void BindKey(int keyName, int realKey) { map.insert_or_assign(keyName, std::pair<int, bool>(realKey, false)); }
	bool KeyStatus(int keyName) { return map[keyName].second; }
};

enum InputKeys
{
	KEY_Shoot = 0,
	KEY_Slow = 1,
	KEY_Up = 2,
	KEY_Down = 3,
	KEY_Left = 4,
	KEY_Right = 5,
	KEY_Action1 = 6,
	KEY_Action2 = 7,
	KEY_Action3 = 8,
	KEY_Action4 = 9,
	KEY_Action5 = 10,
};

enum CollisionLayers
{
	CLL_Player = 1,
	CLL_PlayerBullet = 1 << 1,
	CLL_Enemy = 1 << 2,
	CLL_EnemyBullet = 1 << 3
};

//#define DEBUG_COLLISIONS