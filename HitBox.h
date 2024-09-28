#pragma once
#include<memory>
#include"glm/glm.hpp"
#include"UBHS_Definitions.h";
#include"Sprite.h"

class HitBox
{
	protected:
		
		bool AABB2Collision(Entity& owner, Entity& target);
		bool CircleAABBCollision(Entity& circle, Entity& box);
		bool Circle2Collision(Entity& first, Entity& second);

	public:

		enum Type
		{
			T_Circle = 0,
			T_AABB = 1,			
		};

		bool active = true;
		Type type;
		int collisionLayer;
		int collidesWithLayers;
		glm::vec2 scale;
		glm::vec2 pivot;

		void SetCollisionLayers(std::initializer_list<int> args);

		void DebugDrawHitbox(glm::mat4 projection, std::shared_ptr<Sprite> sprite, Entity& owner);

		bool Collision(Entity& owner, Entity& target);

		inline HitBox(Type type, glm::vec2 scale, glm::vec2 pivot, int collisionLayer, int collidesWithLayers)
		{
			this -> type = type;
			this -> scale = scale; 
			this -> pivot = pivot;
			this -> collisionLayer = collisionLayer; 
			this -> collidesWithLayers = collidesWithLayers;
		}
		inline HitBox(Type type, glm::vec2 scale, int collisionLayer, int collidesWithLayers) : HitBox(type, scale, glm::vec2(0.5f), collisionLayer, collidesWithLayers) {  }

};

