#pragma once
#include"ManualTargetedMovement.h"

class SimpleMovement : public ManualTargetedMovement
{
	using ManualTargetedMovement::ManualTargetedMovement;

	public:
		inline virtual void SetMovementData(glm::vec2* position, glm::vec2* scale, float* rotation, double t, double dt)
		{
			glm::vec2 position_c = glm::vec2(*position);
			if (!ReachedDestination(position_c, dt)) MoveToDestination(position, position_c, dt);
			else *position = _destination;
		}

		inline SimpleMovement(glm::vec2 destination, float moveSpeed) : ManualTargetedMovement(destination, moveSpeed) {  }
		inline SimpleMovement(glm::vec2 destination, float moveSpeed, float overshoot) : ManualTargetedMovement(destination, moveSpeed, overshoot) {  }

		inline MovementController* Clone() { return new SimpleMovement(*this); }
};
