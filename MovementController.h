#pragma once
#include"glm/glm.hpp"

class MovementController
{
	public: 
		virtual void SetMovementData(glm::vec2* position, glm::vec2* scale, float* rotation, double t, double dt) = 0;
		virtual MovementController* Clone() = 0;
};

