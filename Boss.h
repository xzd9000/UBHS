#pragma once
#include<vector>
#include"Character.h"
#include"Timer.h"

struct BossPhase
{

	float hp = 100.f;
	float duration = -1.f;
	float newEventTimelineTime = -1.f;
	bool unfreezeTimerOnEnd;
	std::unique_ptr<MovementController>  move;
	std::unique_ptr<AttackAction>      attack;
	std::unique_ptr<AttackTime>      nextTime;
	std::forward_list<Projectile> projectiles;

	inline BossPhase(float hp, float duration, float newEventTime, bool unfreezeTimer, std::unique_ptr<MovementController>&& move,
		std::unique_ptr<AttackAction>&& attack,
		std::unique_ptr<AttackTime>&& nextTime)
	{
		this -> hp = hp;
		this -> duration = duration;
		this -> move = std::move(move);
		this -> attack = std::move(attack);
		this -> nextTime = std::move(nextTime);
		newEventTimelineTime = newEventTime;
		unfreezeTimerOnEnd = unfreezeTimer;
	}
	inline BossPhase(float hp, float duration, float newEventTime, bool unfreezeTimer, std::unique_ptr<MovementController>&& move,
		std::unique_ptr<AttackAction>&& attack,
		std::unique_ptr<AttackTime>&& nextTime,
		std::initializer_list<Projectile> projectiles)
		: BossPhase(hp, duration, newEventTime, unfreezeTimer, std::move(move), std::move(attack), std::move(nextTime))
	{
		this -> projectiles.insert_after(this -> projectiles.before_begin(), projectiles);
	}
	inline BossPhase(const BossPhase& other) { operator=(other); }
	inline BossPhase& operator=(const BossPhase& other)
	{
		if (&other != this)
		{
			AssignBase(other);
			if (other.move != nullptr)     move.reset(other.move     -> Clone());
			if (other.attack != nullptr)   attack.reset(other.attack   -> Clone());
			if (other.nextTime != nullptr) nextTime.reset(other.nextTime -> Clone());
			if (!other.projectiles.empty())
			{
				projectiles.clear();
				projectiles.insert_after(projectiles.before_begin(), other.projectiles.begin(), other.projectiles.end());
			}
		}
		return *this;
	}
	inline BossPhase(BossPhase&& other) { operator=(std::move(other)); }
	inline BossPhase& operator=(BossPhase&& other)
	{
		if (&other != this)
		{
			AssignBase(std::move(other));
			move.reset(other.move.release());
			attack.reset(other.attack.release());
			nextTime.reset(other.nextTime.release());
			if (!other.projectiles.empty())
			{
				projectiles.clear();
				projectiles.insert_after(projectiles.before_begin(), other.projectiles.begin(), other.projectiles.end());
				other.projectiles.clear();
			}
		}
		return *this;
	}
	private: 
		void AssignBase(const BossPhase& other) 
		{  
			hp = other.hp;
			duration = other.duration;
			newEventTimelineTime = other.newEventTimelineTime;
			unfreezeTimerOnEnd = other.unfreezeTimerOnEnd;
		}
		void AssignBase(BossPhase&& other)
		{
			hp = std::move(other.hp);
			duration = std::move(other.duration);
			newEventTimelineTime = std::move(other.newEventTimelineTime);
			unfreezeTimerOnEnd = std::move(other.unfreezeTimerOnEnd);
		}
};

class Boss : public Character
{
	using Phase = BossPhase;

	private:
		std::forward_list<Phase> _phases;
		Timer* _timer;

		double _phaseStart = 0.0;
		double _phaseEnd = 0.0;
		double _nextEventTime = -1.0;
		bool   _unfreezeNext = false;

		inline void CheckHP() { if (_hp <= 0.0) NextPhase(); }

	public:
		inline void NextPhase()
		{
			if (_nextEventTime >= 0) _timer -> timeEvent = _nextEventTime;
			if (_unfreezeNext) _timer -> eventTimeFrozen = false;

			if (!_phases.empty())
			{
				Phase& phase = *_phases.begin();
			   _hp = phase.hp;
				if (attackController == nullptr) attackController = std::unique_ptr<AttackController>(new AttackController());

				if (!phase.projectiles.empty()) attackController -> CopyProjectiles(phase.projectiles.begin(), phase.projectiles.end());

				moveController.reset(phase.move.release());
				attackController -> attack.reset  (phase.attack.release());
				attackController -> nextTime.reset(phase.nextTime.release());

			   _phaseStart = _timer -> timeWorld;
			   _phaseEnd = phase.duration >= 0 ? _phaseStart + phase.duration : -1.f;

			   _nextEventTime = phase.newEventTimelineTime;
			   _unfreezeNext = phase.unfreezeTimerOnEnd;

			   _phases.pop_front();
			}
			else active = false;
		}
		inline void ProcessMovement(double t, double dt) override
		{
			if (active && attackController == nullptr && moveController == nullptr) NextPhase();
			else if (active && _phaseEnd >= 0.0 && t >= _phaseEnd) NextPhase(); 
			else Character::ProcessMovement(t, dt);
		}

		inline Boss(std::shared_ptr<Sprite> sprite, std::unique_ptr<HitBox> hitbox, float hp, std::initializer_list<Phase> phases) : Character(sprite, nullptr, hp)
		{
			_hitbox.reset(hitbox.release());
			_hp = hp;
			attackController = nullptr;
			auto p = phases.begin();
			auto pn = _phases.before_begin();
			for (; p != phases.end(); ++p, ++pn) _phases.insert_after(pn, *p);
		}
		inline Boss(const Character& other, std::initializer_list<Phase> phases) : Character(other)
		{
			attackController = nullptr;
			moveController = nullptr;
			auto p = phases.begin();
			auto pn = _phases.before_begin();
			for (; p != phases.end(); ++p, ++pn) _phases.insert_after(pn, *p);
		}

		inline Boss(const Boss& other) : Character(other)
		{
			moveController = nullptr;
			attackController = nullptr;
			auto p = other._phases.begin();
			auto pn = _phases.before_begin();
			for (; p != other._phases.end(); ++p, ++pn) _phases.insert_after(_phases.before_begin(), *p);
		}
		inline Boss& operator=(const Boss& other)
		{
			if (&other != this)
			{
				Entity::operator=(other);
				moveController = nullptr;
				attackController = nullptr;
				auto p = other._phases.begin();
				auto pn = _phases.before_begin();
				for (; p != other._phases.end(); ++p, ++pn) _phases.insert_after(_phases.before_begin(), *p);
			}
			return *this;
		}
};
