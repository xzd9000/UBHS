#pragma once
#include"MovementController.h"

class ManualTargetedMovement : public MovementController
{
	protected:
		glm::vec2 _destination;
		float _moveSpeed;
		float _overshootFactor = 1.04f;

		inline bool ReachedDestination(glm::vec2 current, double dt) { return glm::distance(current, _destination) <= _moveSpeed * (float)dt * _overshootFactor; }
		inline void MoveToDestination(glm::vec2* position, glm::vec2 current, double dt) { *position = glm::vec2(current + glm::normalize(_destination - current) * _moveSpeed * (float)dt); }

		inline ManualTargetedMovement(glm::vec2 destination, float moveSpeed) { _destination = destination; _moveSpeed = moveSpeed; }
		inline ManualTargetedMovement(glm::vec2 destination, float moveSpeed, float overshoot) : ManualTargetedMovement(destination, moveSpeed) { _overshootFactor = overshoot; }
};