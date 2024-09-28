#include"HitBox.h"
#include"Entity.h"

using std::initializer_list;
using glm::vec2;
using glm::vec3;
using glm::mat4;
using glm::clamp;
using glm::length;
using std::shared_ptr;

void HitBox::SetCollisionLayers(initializer_list<int> args)
{
	collidesWithLayers = 0;
	for (auto i = args.begin(); i != args.end(); ++i) collidesWithLayers |= *i;
}

bool HitBox::Collision(Entity& owner, Entity& target)
{
	if (owner.active && target.active && owner.hitbox() -> active && target.hitbox() -> active) 
	{
		if ((target.hitbox() -> collidesWithLayers & owner.hitbox() -> collisionLayer) > 0)
		{
			     if (owner.hitbox() -> type == T_AABB   && target.hitbox() -> type == T_AABB  ) return AABB2Collision(owner, target);
			else if (owner.hitbox() -> type == T_AABB   && target.hitbox() -> type == T_Circle) return CircleAABBCollision(target, owner);
			else if (owner.hitbox() -> type == T_Circle && target.hitbox() -> type == T_AABB  ) return CircleAABBCollision(owner, target);
			else if (owner.hitbox() -> type == T_Circle && target.hitbox() -> type == T_Circle) return Circle2Collision(owner, target);
		}
	}
	return false;
}

bool HitBox::AABB2Collision(Entity& first, Entity& second)
{
	HitBox& box1 = * first.hitbox();
	HitBox& box2 = *second.hitbox();
	vec2 alignedTopLeft1(first.position.x - box1.pivot.x * first.scale.x * box1.scale.x,
		                 first.position.y - box1.pivot.y * first.scale.y * box1.scale.y),
		 alignedTopLeft2(second.position.x - box2.pivot.x * second.scale.x * box2.scale.x,
		                 second.position.y - box2.pivot.y * second.scale.y * box2.scale.y);
	return alignedTopLeft1.x + first.scale.x * box1.scale.x >= alignedTopLeft2.x && alignedTopLeft2.x + second.scale.x * box2.scale.x >= alignedTopLeft1.x &&
		   alignedTopLeft1.y + first.scale.y * box1.scale.y >= alignedTopLeft2.y && alignedTopLeft2.y + second.scale.y * box2.scale.y >= alignedTopLeft1.y;
}

bool HitBox::CircleAABBCollision(Entity& circle, Entity& box)
{
	HitBox& aabb = *box.hitbox();
	HitBox& cbox = *circle.hitbox();
	vec2 boxHalfs(box.scale.x * aabb.scale.x * 0.5f, box.scale.y * aabb.scale.y * 0.5f);
	vec2 boxCenter(box.position.x - aabb.pivot.x * box.scale.x * aabb.scale.x + 0.5f * box.scale.x * aabb.scale.x,
		           box.position.y - aabb.pivot.y * box.scale.y * aabb.scale.y + 0.5f * box.scale.y * aabb.scale.y);

	vec2 circleCenter(circle.position.x - cbox.pivot.x * cbox.scale.x * circle.scale.x + 0.5f * circle.scale.x * cbox.scale.x, 
					  circle.position.y - cbox.pivot.y * cbox.scale.x * circle.scale.x + 0.5f * circle.scale.x * cbox.scale.x);

	vec2 difference = circleCenter - boxCenter;
	vec2 closest = boxCenter + clamp(difference, -boxHalfs, boxHalfs);
		 difference = closest - circleCenter;

	return length(difference) < 0.5f * circle.scale.x * cbox.scale.x;
}

bool HitBox::Circle2Collision(Entity& first, Entity& second)
{
	HitBox& circle1 = * first.hitbox();
	HitBox& circle2 = *second.hitbox();

	vec2 center1(first.position.x - circle1.pivot.x * circle1.scale.x * first.scale.x + 0.5f * first.scale.x * circle1.scale.x,
				 first.position.y - circle1.pivot.y * circle1.scale.x * first.scale.x + 0.5f * first.scale.x * circle1.scale.x);

	vec2 center2(second.position.x - circle2.pivot.x * circle2.scale.x * second.scale.x + 0.5f * second.scale.x * circle2.scale.x,
				 second.position.y - circle2.pivot.y * circle2.scale.x * second.scale.x + 0.5f * second.scale.x * circle2.scale.x);

	return length(center1 - center2) <= 0.5f *  first.scale.x * circle1.scale.x + 
										0.5f * second.scale.x * circle2.scale.x;
}

void HitBox::DebugDrawHitbox(glm::mat4 projection, std::shared_ptr<Sprite> sprite, Entity& owner)
{
#ifdef DEBUG_COLLISIONS
	glm::vec2 hitBoxScale;

	     if (type == T_AABB) hitBoxScale = vec2(owner.scale.x * scale.x, owner.scale.y * scale.y);
	else if (type == T_Circle) hitBoxScale = vec2(owner.scale.x * scale.x);

	sprite -> Draw(projection, vec3(1.f), glm::vec2(owner.position.x,
													owner.position.y), hitBoxScale, pivot, 0.f);

#endif
}