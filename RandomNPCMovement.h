#pragma once
#include"ManualTargetedMovement.h"
#include<random>

class RandomNPCMovement : public ManualTargetedMovement
{
	using ManualTargetedMovement::ManualTargetedMovement;
	using vec4xxyy = glm::vec4;

	glm::vec4 _movementRange;
	double _waitUntil = 0.0;
	double _maxWait = 0.0;

	public:
		virtual void SetMovementData(glm::vec2* position, glm::vec2* scale, float* rotation, double t, double dt)
		{
			glm::vec2 position_c = glm::vec2(*position);
			if (_waitUntil > t) {  }
			else if (!ReachedDestination(position_c, dt)) MoveToDestination(position, position_c, dt);
			else
			{
				*position = _destination;

				std::random_device rd;
				std::ranlux24_base genX(rd()), genY(rd()), genT(rd());
				std::uniform_real_distribution<float>  moveX(_movementRange.x, _movementRange.y), moveY(_movementRange.z, _movementRange.w);
				std::uniform_real_distribution<double> wait(0.0, _maxWait);
				_destination = glm::vec2(moveX(genX), moveY(genY));
				_waitUntil = t + wait(genT);
			}
		}

		RandomNPCMovement(vec4xxyy movementRange, glm::vec2 destination0, double maxWait, float moveSpeed, float overshoot) : ManualTargetedMovement(destination0, moveSpeed, overshoot)
		{
			_movementRange = movementRange;
			_maxWait = maxWait;
		}
		RandomNPCMovement(vec4xxyy movementRange, glm::vec2 destination0, double maxWait, float moveSpeed) : ManualTargetedMovement(destination0, moveSpeed)
		{
			_movementRange = movementRange;
			_maxWait = maxWait;
		}

		inline MovementController* Clone() { return new RandomNPCMovement(*this); }
};