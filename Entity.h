#pragma once
#include<memory>
#include"glm/glm.hpp"
#include"Sprite.h"
#include"UBHS_Definitions.h"
#include"HitBox.h"

class Entity
{	
	protected:
		std::shared_ptr<Sprite> _sprite = nullptr;
		std::unique_ptr<HitBox> _hitbox = nullptr;

		bool _delete = false;

		inline void AssignBase(const Entity& other)
		{
		   _sprite = other._sprite;
		   _delete = other._delete;
			position = other.position;
			scale = other.scale;
			pivot = other.pivot;
			spriteColor = other.spriteColor;
			rotation = other.rotation;
			activationTime = other.activationTime;
			active = other.active;
			checkCollisions = other.checkCollisions;
			collisionFunc = other.collisionFunc;
		}

	public:
		glm::vec2 position = glm::vec2(0.f);
		glm::vec2 scale = glm::vec2(10.f);
		glm::vec2 pivot = glm::vec2(0.5f);
		glm::vec3 spriteColor = glm::vec3(1.f);
		float     rotation = 0.f;
		double    activationTime = 0.f;
		bool      active = true;
		bool      checkCollisions = false;
		CollisionFunc collisionFunc = nullptr;

		inline				bool deleteAllowed() { return  _delete; }
		inline					Sprite& sprite() { return *_sprite; }
		inline std::unique_ptr<HitBox>& hitbox() { return  _hitbox; }

		inline void Activate(double activationTime) { active = true; Entity::activationTime = activationTime; }
		inline void OnCollision(double t, double dt, Entity& owner, Entity& target) { if (collisionFunc != nullptr) collisionFunc(t, dt, owner, target); }

		inline virtual void ProcessMovement(double t, double dt) = 0;
		inline virtual void Draw(glm::mat4 projection) { if (active && _sprite != nullptr) _sprite -> Draw(projection, spriteColor, position, scale, pivot, rotation); }

		inline Entity(std::shared_ptr<Sprite> sprite) { _sprite = sprite; }
		inline Entity(std::shared_ptr<Sprite> sprite, std::unique_ptr<HitBox> hitbox) : Entity(sprite) { _hitbox.reset(hitbox.release()); }
		inline Entity(std::shared_ptr<Sprite> sprite, glm::vec2 position, glm::vec2 scale, glm::vec2 pivot, glm::vec3 spriteColor, float rotation) : Entity(sprite)
		{
			Entity::position = position;
			Entity::scale = scale;
			Entity::pivot = pivot;
			Entity::spriteColor = spriteColor;
			Entity::rotation = rotation;
		}
		inline Entity(std::shared_ptr<Sprite> sprite, std::unique_ptr<HitBox> hitbox, glm::vec2 position, glm::vec2 scale, glm::vec2 pivot, glm::vec3 spriteColor, float rotation) : Entity(sprite, position, scale, pivot, spriteColor, rotation) { _hitbox.reset(hitbox.release()); }
		Entity() {  }

		inline Entity		    (const Entity& other) 
		{ 
			if (other._hitbox != nullptr) _hitbox = std::unique_ptr<HitBox>(new HitBox(*other._hitbox));
			else _hitbox = nullptr;

			AssignBase(other);
		};
		inline Entity& operator=(const Entity& other)
		{
			if (this != &other) 
			{
				if (other._hitbox != nullptr)
				{
					if (_hitbox != nullptr) *_hitbox = HitBox(*other._hitbox);
					else _hitbox = std::unique_ptr<HitBox>(new HitBox(*other._hitbox));
				}
				else _hitbox = nullptr;

				AssignBase(other);
			}
			return *this;
		};

		inline Entity(Entity&& other)
		{
			_hitbox.reset(other._hitbox.release());

			AssignBase(other);
		}
		inline Entity& operator=(Entity&& other) 
		{
			if (this != &other)
			{
				_hitbox.reset(other._hitbox.release());
				AssignBase(other);
			}
			return *this;
		}
};