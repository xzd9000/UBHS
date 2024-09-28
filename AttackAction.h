#pragma once
#include<vector>
#include<random>
#include"glm/glm.hpp"
#include"AttackController.h"
#include"EntityList.h"

struct SimpleAttackAction : AttackAction
{
	inline SimpleAttackAction(EntityList* entities) { this -> entities = entities; }
	inline AttackAction* Clone() { return new SimpleAttackAction(*this); }

	inline void Action(Character& owner, AttackController& c, double t, double dt)
	{
		for (auto p = c.projectiles.begin(); p != c.projectiles.end(); ++p)
		{
			Projectile& proj = entities -> ActivateProjectile(*p, t);
			proj.position += owner.position;
		}
	}

};
using SimpleAttack = SimpleAttackAction;
using SimpleAA     = SimpleAttackAction;

struct CircleAttackAction : AttackAction
{
	const float pi = std::_Pi;

	float projCount = 10.f;
	float projSpeed = 300.f;
	float phasing = 0.f;
	float initAngle;

	inline void Action(Character& owner, AttackController& c, double t, double dt)
	{
		for (float rad = initAngle; rad > (pi * -2.f); rad -= pi / projCount)
		{
			Projectile& proj = entities -> ActivateProjectile(*(c.projectiles.begin()), t);
			proj.velocity = glm::vec2(glm::cos(rad), glm::sin(rad)) * projSpeed;
			proj.position = owner.position;
		}
		initAngle += phasing;
	}

	inline CircleAttackAction(float projCount, float projSpeed, EntityList* entities, float phasing = 0.f)
	{
		this -> projCount = projCount;
		this -> projSpeed = projSpeed;
		this -> entities = entities;
		this -> phasing = phasing;
		initAngle = pi;
	}
	inline AttackAction* Clone() { return new CircleAttackAction(*this); }
};
using CircleAttack = CircleAttackAction;
using CircleAA     = CircleAttackAction;

struct TargetedAttackBase : AttackAction
{
	std::shared_ptr<Character> player;
	float projSpeed = 300.f;

	inline virtual glm::vec2 DirectionTo(glm::vec2 other, glm::vec2 this_) { return glm::normalize(other - this_); }
	inline void Action(Character& owner, AttackController& c, double t, double dt)
	{
		glm::vec2 dir;
		for (auto p = c.projectiles.begin(); p != c.projectiles.end(); ++p)
		{
			Projectile& proj = entities -> ActivateProjectile(*p, t);
			dir = DecideDirection(owner, c, t, dt);
			proj.velocity = dir * projSpeed;
			proj.position += owner.position;
		}
	}

	inline TargetedAttackBase(float projSpeed, EntityList* entities)
	{
		this -> player = player;
		this -> projSpeed = projSpeed;
		this -> entities = entities;
	}

	protected: inline virtual glm::vec2 DecideDirection(Character& owner, AttackController& c, double t, double dt) = 0;
};

struct TargetedAttackAction : TargetedAttackBase
{
	std::shared_ptr<Character> player;

	inline TargetedAttackAction(std::shared_ptr<Character> player, float projSpeed, EntityList* entities) : TargetedAttackBase(projSpeed, entities) { this -> player = player; }
	inline AttackAction* Clone() { return new TargetedAttackAction(*this); }

	protected: inline virtual glm::vec2 DecideDirection(Character& owner, AttackController& c, double t, double dt) { return DirectionTo(player -> position, owner.position); }
};
using TargetedAttack = TargetedAttackAction;
using TargetedAA	 = TargetedAttackAction;

struct FixedTargetAttackAction : TargetedAttackBase
{
	glm::vec2 target;

	inline FixedTargetAttackAction(glm::vec2 target, float projSpeed, EntityList* entities) : TargetedAttackBase(projSpeed, entities) { this -> target = target; }
	inline AttackAction* Clone() { return new FixedTargetAttackAction(*this); }

	protected: inline virtual glm::vec2 DecideDirection(Character& owner, AttackController& c, double t, double dt) { return DirectionTo(target, owner.position); }
};
using FixedTargetAttack = FixedTargetAttackAction;
using FixedTargetAA     = FixedTargetAttackAction;

struct RandomAttackAction : AttackAction
{
	using iterator = decltype(AttackController::projectiles.begin());

	const float pi = std::_Pi;

	float projectileSpeed;

	inline RandomAttackAction(EntityList* entities, float projectileSpeed) 
	{ 
		this -> entities = entities;
		this -> projectileSpeed = projectileSpeed;
	}
	inline AttackAction* Clone() { return new RandomAttackAction(*this); }

	inline void Action(Character& owner, AttackController& c, double t, double dt)
	{
		if (!initialized)
		{
			for (auto p = c.projectiles.begin(); p != c.projectiles.end(); ++p) iters.push_back(p);
			initialized = true;
		}

		std::ranlux24_base genAngle  = std::ranlux24_base(std::random_device()());
		std::ranlux24_base genSelect = std::ranlux24_base(std::random_device()());
		float angle  = std::uniform_real_distribution<float> (0.f, pi * 2.f)      (genAngle);
		int   select = std::uniform_int_distribution <int>   (0, iters.size() - 1)(genSelect);

		Projectile& p = entities -> ActivateProjectile(*(iters[select]), t);
		p.position = owner.position;
		p.velocity = glm::vec2(glm::cos(angle), glm::sin(angle)) * projectileSpeed;
	}

	protected: 
		std::vector<iterator> iters;
		bool initialized = false;
};
using RandomAttack = RandomAttackAction;
using RandomAA	   = RandomAttackAction;