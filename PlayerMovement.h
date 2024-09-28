#pragma once
#include<forward_list>
#include"MovementController.h"
#include"UBHS_Definitions.h"
#include"EntityList.h"


class PlayerMovement : public MovementController
{
	private:
		float _moveSpeed = 0.f;
		float _slowMultiplier = 1.f;
		glm::vec2 _direction = glm::vec2(0.f);

		float _windowHeight;
		float _windowWidth;
		Keymap* _keymap;
		EntityList* _entityList;

		std::forward_list<Projectile> _projectiles;

		bool _shotFired = false;
		bool _shootPressed = false;
		double _shootStartTime = -1.0;

	public:
		inline void SetMovementData(glm::vec2* position, glm::vec2* scale, float* rotation, double t, double dt) 
		{
			glm::vec2 newPosition = glm::vec2(*position);
			_direction = glm::vec2(0 + (int)(_keymap -> KeyStatus(KEY_Left)) * -1 + (int)(_keymap -> KeyStatus(KEY_Right)),
				                   0 + (int)(_keymap -> KeyStatus(KEY_Down)) * -1 + (int)(_keymap -> KeyStatus(KEY_Up)));

			newPosition += _direction * _moveSpeed * (float)dt * (_keymap -> KeyStatus(KEY_Slow) ? _slowMultiplier : 1.f);

			if (newPosition.x < 0.f	|| newPosition.x > _windowWidth ) newPosition.x = position -> x;
			if (newPosition.y < 0.f	|| newPosition.y > _windowHeight) newPosition.y = position -> y;

			 *position = newPosition;

			if (!_keymap -> KeyStatus(KEY_Shoot)) _shootPressed = false;
			else if (_shootPressed == false)
			{
				_shootPressed = true;
				_shootStartTime = t;
			}

			if (_shootPressed)
			{
				if (((int)((t - _shootStartTime) * 1000.0) % 250 <= 50))
				{
					if (!_shotFired)
					{
						for (auto proj = _projectiles.begin(); proj != _projectiles.end(); ++proj)
						{
							Projectile& shot = _entityList -> ActivateProjectile(*proj, t);
							shot.position += *position;
						}
						_shotFired = true;
					}
				}
				else _shotFired = false;
			}
		}
		inline void SetDirection(glm::vec2 direction) { _direction = direction; }

		inline PlayerMovement(float moveSpeed, float slowMultiplier, Keymap* keymap, EntityList* list, float windowHeight, float windowWidth) 
		{ 
			_moveSpeed = moveSpeed;
			_slowMultiplier = slowMultiplier;
			_keymap = keymap;
			_entityList = list;
			_windowHeight = windowHeight;
			_windowWidth = windowWidth;
		}
		inline PlayerMovement(float moveSpeed, float slowMultiplier, Keymap* keymap, EntityList* list, float windowHeight, float windowWidth, std::initializer_list<Projectile> projs) : PlayerMovement(moveSpeed, slowMultiplier, keymap, list, windowHeight, windowWidth)
		{
			for (auto i = projs.begin(); i != projs.end(); ++i) _projectiles.push_front(*i);
		}

		inline MovementController* Clone() { return new PlayerMovement(*this); }
};

