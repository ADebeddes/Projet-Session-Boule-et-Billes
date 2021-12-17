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
		vector<std::string> modele;
	
		EntityManager(int nbEnemies, PxVec3 _PremierePosition,  CDispositifD3D11* pDispositif);
		~EntityManager();
		void AddEntity(PxVec3 PremierePosition, CDispositifD3D11* pDispositif);
		void DelEntity();
		void AnimeEntities();
		void resetEntities();
		void respawnEntities();
	};

}

