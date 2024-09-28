#pragma once
#include"Entity.h"
#include"UBHS_Definitions.h"

class Projectile : public Entity
{
	using Entity::Entity;

	private:
		inline void InitConstructor(glm::vec2 initPosition, glm::vec2 initVelocity, AccelerationFunc accelerationFunc)
		{
			position = initPosition;
		    velocity = initVelocity;
		    acceleraionFunc = accelerationFunc;
		}
		inline void InitConstructor(glm::vec2 initPosition, glm::vec2 initVelocity) { InitConstructor(initPosition, initVelocity, [](double, double, glm::vec2) { return glm::vec2(0.f, 0.f); }); }

		inline void AssignBase(const Projectile& other)
		{
			velocity = other.velocity;
			acceleration = other.acceleration;
			acceleraionFunc = other.acceleraionFunc;
		}

	public:
		glm::vec2 velocity;
		glm::vec2 acceleration;
		AccelerationFunc acceleraionFunc;

		inline virtual void ProcessMovement(double t, double dt)
		{
			if (active) 
			{
				if (acceleraionFunc != nullptr) acceleration = acceleraionFunc(t - activationTime, dt, position);
				velocity += acceleration * (float)dt;
				position += velocity * (float)dt;
			}
		}

		inline Projectile(std::shared_ptr<Sprite> sprite, glm::vec2 initPosition, glm::vec2 initVelocity, AccelerationFunc accelerationFunc) : Entity(sprite) { InitConstructor(initPosition, initVelocity, accelerationFunc); }
		inline Projectile(std::shared_ptr<Sprite> sprite, glm::vec2 initPosition, glm::vec2 initVelocity) : Entity(sprite) { InitConstructor(initPosition, initVelocity); }
		inline Projectile(std::shared_ptr<Sprite> sprite, std::unique_ptr<HitBox> hitbox, glm::vec2 initPosition, glm::vec2 initVelocity, AccelerationFunc accelerationFunc) : Projectile(sprite, initPosition, initVelocity, accelerationFunc) { _hitbox.reset(hitbox.release()); }
		inline Projectile(std::shared_ptr<Sprite> sprite, std::unique_ptr<HitBox> hitbox, glm::vec2 initPosition, glm::vec2 initVelocity) : Projectile(sprite, initPosition, initVelocity) { _hitbox.reset(hitbox.release()); }
		inline Projectile(std::shared_ptr<Sprite> sprite, std::unique_ptr<HitBox> hitbox, glm::vec2 initPosition, glm::vec2 initVelocity, AccelerationFunc accelerationFunc, glm::vec2 scale, glm::vec2 pivot, glm::vec3 spriteColor, float rotation) : Entity(sprite, initPosition, scale, pivot, spriteColor, rotation)
		{
			_hitbox.reset(hitbox.release());

			velocity = initVelocity;
			acceleraionFunc = accelerationFunc;
		}

		inline Projectile			(const Projectile& other) : Entity(other) { AssignBase(other); }
		inline Projectile& operator=(const Projectile& other)
		{
			Entity::operator=(other);
			AssignBase(other);
			return *this;
		}
		inline Projectile			(Projectile&& other) : Entity(std::move(other)) { AssignBase(other); }
		inline Projectile& operator=(Projectile&& other)
		{
			Entity::operator=(std::move(other));
			AssignBase(other);
			return *this;
		}
};