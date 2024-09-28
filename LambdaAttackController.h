#pragma once
#include"AttackController.h"
#include"UBHS_Definitions.h"

class LambdaAttackController : public AttackController
{
	public:
		AttackFunc attackFunc	= nullptr;
		AttackFunc nextTimeFunc = nullptr;

		inline void ProcessAttacks(Entity& owner, double t, double dt)
		{
			if (t >= nextAttackTime)
			{
				if (attackFunc != nullptr)   attackFunc(owner, *this, t, dt);
				if (nextTimeFunc != nullptr) nextTimeFunc(owner, *this, t, dt);
			}
		}

		inline LambdaAttackController(AttackFunc attack, AttackFunc nextTime) : AttackController()
		{
			attackFunc = attack;
			nextTimeFunc = nextTime;
		}

		inline LambdaAttackController			(const LambdaAttackController& other) { operator=(other); }
		inline LambdaAttackController& operator=(const LambdaAttackController& other)
		{
			CopyProjectiles(other);
			attackFunc   = other.attackFunc;
			nextTimeFunc = other.nextTimeFunc;
		}
};
