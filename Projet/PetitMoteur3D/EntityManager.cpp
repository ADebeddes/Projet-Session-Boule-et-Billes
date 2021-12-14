#include "stdafx.h"
#include "EntityManager.h"
#include "Sphere.h"
#include "MoteurWindows.h"

namespace PM3D {

	EntityManager::EntityManager(int nbEnemies, PxVec3 PremierePosition, CDispositifD3D11* pDispositif)
	{
		pPlayer = new Player(PremierePosition, PxVec3(0, -1, 0), *(new Sphere(pDispositif, "Boule.obj", "Player")));
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		pPlayer->playerCharacter.SetTexture(rMoteur.GetTextureManager().GetNewTexture(L"snow.dds", pDispositif));

		for (int i = 1; i <= nbEnemies; i++) {
			auto name = new string("Enemy"s + to_string(i));



			enemies.push_back(
				new Enemy(
					PremierePosition + PxVec3(5.0f, 0, 0) * static_cast<float>(i)
					, *(new Sphere(pDispositif, "Boule.obj", const_cast<char*>((*name).c_str())))
					, (*name).c_str()));
			enemies[i - 1]->enemyCharacter.SetTexture(rMoteur.GetTextureManager().GetNewTexture(L"granite.dds", pDispositif));
		}
	}
	void EntityManager::AddEntity(PxVec3 PremierePosition, CDispositifD3D11* pDispositif)
	{
		if (enemies.size() < 20) {
			CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();

			auto name = new string("Enemy"s + to_string(enemies.size()));
			enemies.push_back(
				new Enemy(
					PremierePosition + PxVec3(5.0f, 0, 0) * static_cast<float>(enemies.size())
					, *(new Sphere(pDispositif, "Boule.obj", const_cast<char*>((*name).c_str())))
					, (*name).c_str()));
			enemies[enemies.size() - 1]->enemyCharacter.SetTexture(rMoteur.GetTextureManager().GetNewTexture(L"granite.dds", pDispositif));

		}
	}
	void EntityManager::DelEntity()
	{
		int t = 0;
		if (!enemies.empty())
		enemies.pop_back();
	}
	void EntityManager::AnimeEntities()
	{
		pPlayer->Anime();
		std::for_each(begin(enemies), end(enemies), [](auto e) {e->Anime(); });
	}
}
