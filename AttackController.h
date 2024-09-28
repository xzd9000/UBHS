#pragma once
#include<forward_list>
#include<memory>
#include"UBHS_Definitions.h"
#include"Projectile.h"

struct AttackAction { EntityList* entities; virtual void Action(Character& owner, AttackController& c, double t, double dt) = 0; virtual AttackAction* Clone() = 0; };
struct AttackTime   { virtual void NextTime(Character& owner, AttackController& c, double t, double dt) = 0; virtual AttackTime*   Clone() = 0; };

struct AttackController
{
	double					  nextAttackTime = 0.0;
	std::forward_list<Projectile>	   projectiles;
	std::unique_ptr<AttackAction> attack = nullptr;
	std::unique_ptr<AttackTime> nextTime = nullptr;

	inline virtual void ProcessAttacks(Character& owner, double t, double dt)
	{
		if (t >= nextAttackTime)
		{
			if (attack   != nullptr)   attack ->   Action(owner, *this, t, dt);
			if (nextTime != nullptr) nextTime -> NextTime(owner, *this, t, dt);
		}
	}

	inline AttackController(std::unique_ptr<AttackAction>&& attack, std::unique_ptr<AttackTime>&& nextTime, double firstAttackTime = 0.0)
	{
		this -> attack   = std::move(attack);
		this -> nextTime = std::move(nextTime);
		nextAttackTime   = firstAttackTime;
	}
	AttackController() {  }

	inline AttackController			  (const AttackController& other) { operator=(other); }
	inline AttackController& operator=(const AttackController& other)
	{
		if (&other != this) 
		{
			CopyProjectiles(other.projectiles.begin(), other.projectiles.end());
			nextAttackTime = other.nextAttackTime;
			if (other.attack != nullptr)   attack.reset  (other.attack   -> Clone());
			if (other.nextTime != nullptr) nextTime.reset(other.nextTime -> Clone());
		}
		return *this;
	}

	template <class iter> void CopyProjectiles(iter first, iter last)
	{
		projectiles.clear();
		projectiles.insert_after(projectiles.before_begin(), first, last);
	}
};
