#pragma once
#include"AttackController.h"

struct FixedPeriodAttackTime : public AttackTime
{
	double _attackPeriod;

	inline void NextTime(Character& owner, AttackController& this_, double t, double dt) { this_.nextAttackTime = t + _attackPeriod; }

	inline FixedPeriodAttackTime(double period) { _attackPeriod = period; }

	inline AttackTime* Clone() { return new FixedPeriodAttackTime(*this); }
};
using FixedPeriodAT = FixedPeriodAttackTime;