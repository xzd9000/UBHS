#pragma once
#include<list>
#include"Entity.h"
#include"Projectile.h"
#include"Character.h"

class EntityList
{
	using iterator = std::list<std::shared_ptr<Entity>>::iterator;
	using entity_list = std::list<std::shared_ptr<Entity>>;

	private:		
		entity_list _list;
		
		iterator _projectilesBegin = _list.end();
		iterator _player;

	public:
		
		inline const entity_list& list() { return _list; }

		iterator projectilesBegin() { return _projectilesBegin; }

		inline void AddEntity(std::shared_ptr<Entity> entity) { _list.push_front(entity); }
		inline void AddProjectile(std::shared_ptr<Entity> entity) 
		{  
			if (_projectilesBegin == _list.end()) _list.push_back(entity);
			else _list.insert(_projectilesBegin, entity);
			_projectilesBegin--;
		}
		inline void AddPlayer(std::shared_ptr<Entity> player)
		{
			AddEntity(player);
			_player = _list.begin();
		}

		inline Projectile& ActivateProjectile(Projectile& proj, double activationTime)
		{
			for (auto e = _projectilesBegin; e != _list.end(); e++)
			{	
				if (!((*e) -> active))
				{
					Projectile& ep = static_cast<Projectile&>(*(*e));
					ep = proj;
					ep.Activate(activationTime);
					return ep;
				}
			}
			std::shared_ptr<Entity> eptr = std::make_shared<Projectile>(proj);
			AddProjectile(eptr);
			return static_cast<Projectile&>(*eptr);
		}

		inline void KillAllNPC()
		{
			for (auto e = _list.begin(); e != _projectilesBegin; ++e)
			{
				if (e != _player && typeid(*(*e)) == typeid(Character)) (*e) -> active = false;
			}
		}
		inline void DeactivateAllProjectiles()
		{
			for (auto e = _projectilesBegin; e != _list.end(); ++e) (*e) -> active = false;
		}
};