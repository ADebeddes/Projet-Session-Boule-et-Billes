#include "stdafx.h"
#include "SceneManager.h"
#include "resource.h"
#include "MoteurWindows.h"
#include "EntityManager.h"
#include "AfficheurPanneau.h"
#include "Bonus.h"
#include <random>

namespace PM3D {
	SceneManager::SceneManager()
	{
		std::vector<std::unique_ptr<CObjet3D>> liste;
		zones.emplace(std::make_pair(0, std::move(liste)));
		zonesActives.push_back(0);
	}
	bool SceneManager::createZone(int key)
	{
		std::vector<std::unique_ptr<CObjet3D>> liste;
		zones.emplace(std::make_pair(key, std::move(liste)));
		zonesActives.push_back(key); 
		return true;
	}

	bool SceneManager::addToZone(int key, Terrain* objet) {
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		zones[key].emplace_back(objet);
		rMoteur.Moteur_Physique.gScene->addCollection(*(objet->collection));

		if (key > 1) {
			BasicColider* porte = new BasicColider(500, 100, 1);
			float x = (objet->oi.object.min_x+ objet->oi.object.max_x)/2;
			float z = objet->oi.object.max_z-70;
			porte->place(x, objet->getHeightAt(x, z) + 10, z);
			porte->pred = std::bind(&SceneManager::desactiverZone, this, key-1);
			
			add(porte);
			collider_pool.push_back(porte);
		}
		return false;
	}

	bool SceneManager::addToZone(int key, StaticObject* objet)
	{
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		zones[key].emplace_back(objet);
		rMoteur.Moteur_Physique.gScene->addActor(*(objet->body));
		return false;
	}

	bool SceneManager::addToZone(int key, DynamicObject* objet)
	{
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		zones[key].emplace_back(objet);
		rMoteur.Moteur_Physique.gScene->addActor(*(objet->body));
		return false;
	}

	bool SceneManager::addToZone(Skybox* s)
	{
		zones[0].emplace_back(s);
		return false;
	}

	bool SceneManager::addToUI( CAfficheurSprite* objet)
	{
		UI.emplace_back(objet);
		return false;
	}

	bool SceneManager::addToUI(CAfficheurPanneau* objet)
	{
		UI.emplace_back(objet);
		return false;
	}

	bool SceneManager::add(BasicColider* objet)
	{
		 CMoteurWindows::GetInstance().Moteur_Physique.gScene->addActor(*(objet->body));
		return false;
	}

	bool SceneManager::add(int key, Bonus* bonus)
	{
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		zones[key].emplace_back(bonus);
		rMoteur.Moteur_Physique.gScene->addActor(*(bonus->body));
		bonus_pool.push_back(bonus);
		return true;
	}


	bool SceneManager::addEntities(EntityManager* pEm)
	{
		
			CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
			
			//Ajout a la scene de rendu
			zones[0].emplace_back(&(pEm->pPlayer->playerCharacter));

			//Ajout à la scene Physique
			rMoteur.Moteur_Physique.gScene->addActor(*(pEm->pPlayer->playerCharacter.body));
			for (auto enemy : pEm->enemies) {
				zones[0].emplace_back(&(enemy->enemyCharacter));
				rMoteur.Moteur_Physique.gScene->addActor(*(enemy->enemyCharacter.body));
				;
			}

			return true;
		
	}

	void SceneManager::resetCollider()
	{
		zonesActives.clear();
		zonesActives.push_back(0);
		for (auto& collider : collider_pool) {
			collider->triggered = false;
			
			zonesActives.push_back(collider->number);
		}
	}

	void SceneManager::desactiverZone(int i)
	{
		zonesActives.erase(std::find(zonesActives.begin(), zonesActives.end(), i));
	}
	void SceneManager::placeRandomObstacle(int zone, Terrain* where, string obj, wstring texture)
	{


		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();

		int pos = uniform_int_distribution<int>{ 0, static_cast<int>(where->oi.object.points_.size()) }(prng);

		const float x = where->oi.object.points_[pos].x;
		const float y = where->oi.object.points_[pos].y;
		const float z = where->oi.object.points_[pos].z;
		Obstacle* obs = new Obstacle(rMoteur.pDispositif, obj);
		rMoteur.mtx.lock(); 
		obs->SetTexture(rMoteur.TexturesManager.GetNewTexture(texture, rMoteur.pDispositif));
		rMoteur.mtx.unlock();
		obs->place(x, y, z);
		addToZone(zone, obs);
	}


	void SceneManager::placeRandomBonus(int zone, Terrain* where, Bonus* obs)
	{


		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();

		int pos = uniform_int_distribution<int>{ 0, static_cast<int>(where->oi.object.points_.size()) }(prng);

		const float x = where->oi.object.points_[pos].x;
		const float y = where->oi.object.points_[pos].y + 1;
		const float z = where->oi.object.points_[pos].z;
		obs->place(x, y, z);
		add(zone, obs);
	}
}
