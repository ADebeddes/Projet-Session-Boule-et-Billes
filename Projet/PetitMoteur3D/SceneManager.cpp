#include "stdafx.h"
#include "SceneManager.h"
#include "resource.h"
#include "MoteurWindows.h"
#include "EntityManager.h"
#include "AfficheurPanneau.h"
#include "Bonus.h"

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

	bool SceneManager::addLastZone(int key, Terrain* objet)
	{
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		zones[key].emplace_back(objet);
		rMoteur.Moteur_Physique.gScene->addCollection(*(objet->collection));

		if (key > 1) {
			BasicColider* porte = new BasicColider(500, 100, 1);
			float x = (objet->oi.object.min_x + objet->oi.object.max_x) / 2;
			float z = objet->oi.object.max_z - 70;
			porte->place(x, objet->getHeightAt(x, z) + 10, z);
			porte->pred = std::bind(&SceneManager::desactiverZone, this, key - 1);

			add(porte);
			collider_pool.push_back(porte);
		}

		BasicColider* porte = new BasicColider(500, 500, 1);
		float x = (objet->oi.object.min_x + objet->oi.object.max_x) / 2;
		float z = objet->oi.object.min_z+20;
		porte->place(x, objet->getHeightAt(x, z), z);
		porte->pred = std::bind(&SceneManager::afficherSceneFin, this);

		add(porte);
		collider_pool.push_back(porte);
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

	bool SceneManager::afficherSceneFin()
	{
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		rMoteur.m_Sound->fadeOut2();
		rMoteur.m_Sound->stopWaveFile(rMoteur.m_Sound->Growth);
		rMoteur.m_Sound->growthPlayed = false;
		rMoteur.m_Sound->PlayWaveFile(rMoteur.m_Sound->Victory);
		onMenu = true;
		rMoteur.pAfficheurSprite->onScreen = false;
		rMoteur.pAfficheurPanneau->onScreen = false;
		rMoteur.menuController->AfficheurVictoire->onScreen = true;
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

	void SceneManager::resetBonus()
	{
		for (auto bonus : bonus_pool) {
			bonus->visible = true;
			bonus->triggered = false;	
		}

	}

	void SceneManager::resetEntities()
	{
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();

		
		rMoteur.Moteur_Physique.gScene->removeActor(*(rMoteur.pEntityManager->pPlayer->playerCharacter.body));

		for (auto enemy : rMoteur.pEntityManager->enemies) {
			rMoteur.Moteur_Physique.gScene->removeActor(*(enemy->enemyCharacter.body));
		}

		//zones[0].clear();
		rMoteur.pEntityManager->resetEntities();

	}

	void SceneManager::resetTime()
	{
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		rMoteur.minutePassee = 0;
		rMoteur.secondePassee = 0.0f;
	}

	void SceneManager::resetParty()
	{
		//resetZones();
		resetCollider();
		resetBonus();
		resetEntities();
		resetTime();
	}

	void SceneManager::desactiverZone(int i)
	{
		zonesActives.erase(std::find(zonesActives.begin(), zonesActives.end(), i));
	}
	void SceneManager::placeRandomObstacle(int zone,  Terrain* where, string obj, wstring texture)
	{

		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();

		int pos = (int)(rand() / float(RAND_MAX) * where->oi.object.points_.size());

		const float x = where->oi.object.points_[pos].x;
		const float y = where->oi.object.points_[pos].y;
		const float z = where->oi.object.points_[pos].z;
		Obstacle* obs = new Obstacle(rMoteur.pDispositif, obj);
		obs->SetTexture(rMoteur.TexturesManager.GetNewTexture(texture, rMoteur.pDispositif));
		obs->place(x, y, z);
		addToZone(zone, obs);
	}


	void SceneManager::placeRandomObstacle(int zone, Terrain* where, Obstacle obs)
	{

		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();

		int pos = (int)(rand() / float(RAND_MAX) * where->oi.object.points_.size());

		const float x = where->oi.object.points_[pos].x;
		const float y = where->oi.object.points_[pos].y;
		const float z = where->oi.object.points_[pos].z;
		obs.place(x, y, z);
		addToZone(zone, &obs);
	}
}
