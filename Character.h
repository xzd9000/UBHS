#pragma once
#include<memory>
#include"Entity.h"
#include"MovementController.h"
#include"AttackController.h"

class Character : public Entity
{
	using Entity::Entity;

	protected:
		float _hp;

		virtual void CheckHP() { if (_hp <= 0.f) active = false; }

	public:
		inline float hp() { return _hp; }
		inline void AlterHP(float value) { _hp += value; CheckHP(); }
		inline void SetHP(float value) { _hp = value; CheckHP(); }

		std::unique_ptr<MovementController> moveController = nullptr;
		std::unique_ptr<AttackController>   attackController   = nullptr;

		inline void ProcessMovement(double t, double dt) override
		{
			if (active)
			{
				if (moveController != nullptr) moveController -> SetMovementData(&position, &scale, &rotation, t, dt);
				if (attackController != nullptr) attackController -> ProcessAttacks(*this, t, dt);
			}
		}

		inline Character(std::shared_ptr<Sprite> sprite, std::unique_ptr<MovementController> moveController, float hp) : Entity(sprite) 
		{ 
			_hp = hp;
			if (moveController != nullptr) Character::moveController.reset(moveController.release());
		}
		inline Character(std::shared_ptr<Sprite> sprite, std::unique_ptr<MovementController> moveController, std::unique_ptr<HitBox> hitbox, float hp) : Entity(sprite) 
		{ 
			_hp = hp;
			_hitbox.reset(hitbox.release());
			if (moveController != nullptr) Character::moveController.reset(moveController.release());
		}

		inline Character(const Character& other) : Entity(other)
		{
			_hp = other._hp;
			if (other.attackController != nullptr) attackController = std::unique_ptr<AttackController>(new AttackController(*other.attackController));
			if (other.moveController != nullptr) moveController.reset(other.moveController -> Clone());
			else moveController = nullptr;
		};
		inline Character& operator=(const Character& other)
		{
			if (&other != this) 
			{
				Entity::operator=(other);
				_hp = other._hp;
				
				if (other.attackController != nullptr)
				{
					if (attackController != nullptr) *attackController = *other.attackController;
					else attackController = std::unique_ptr<AttackController>(new AttackController(*other.attackController));
				}
				else moveController = nullptr;

				if (other.moveController != nullptr)
				{
					if (moveController != nullptr) *moveController = *other.moveController;
					else moveController.reset(other.moveController -> Clone());
				}
				else moveController = nullptr;
			}
			return *this;
		};

		inline Character(Character&& other) : Entity(std::move(other))
		{
			_hp = other._hp;
			attackController.reset(other.attackController.release());
			moveController.reset(other.moveController.release());
		}
		inline Character& operator=(Character&& other)
		{
			if (this != &other) 
			{
				Entity::operator=(std::move(other));
				_hp = other._hp;
				attackController.reset(other.attackController.release());
				moveController.reset(other.moveController.release());
			}
		}
};

