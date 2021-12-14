#pragma once
#include "Player.h"
#include "Enemy.h"
#include <algorithm>
namespace PM3D {

	//
//   TEMPLATE : EntityManager
//
//   BUT : Gere toutes les entitées dynamiques presentes dans la scene
//
	class EntityManager
	{
	public:
		Player* pPlayer;
		vector<Enemy*> enemies;
	
		EntityManager(int nbEnemies, PxVec3 _PremierePosition,  CDispositifD3D11* pDispositif);
		void AddEntity(PxVec3 PremierePosition, CDispositifD3D11* pDispositif);
		void DelEntity();
		void AnimeEntities();

		

	};

}

