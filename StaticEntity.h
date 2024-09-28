#pragma once
#include"Entity.h"

class StaticEntity : public Entity 
{ 
	using Entity::Entity;

	public:
		inline StaticEntity() {  }
		inline StaticEntity(std::shared_ptr<Sprite> sprite) : Entity(sprite) {  }
		inline StaticEntity(std::shared_ptr<Sprite> sprite, std::unique_ptr<HitBox> hitbox) : Entity(sprite) { _hitbox.reset(hitbox.release()); }
		inline void ProcessMovement(double t, double dt) {} 

		StaticEntity(const StaticEntity& other) : Entity(other) {  }
		StaticEntity& operator=(const StaticEntity& other) { Entity::operator=(other); }
};