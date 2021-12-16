#include "stdafx.h"
#include "SceneManager.h"
#include "resource.h"
#include "MoteurWindows.h"
#include "EntityManager.h"
#include "AfficheurPanneau.h"
#include "Bonus.h"
#include <utility>
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

	bool SceneManager::addToZone(int key, Terrain* objet, bool is_tunnel) {
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		zones[key].emplace_back(objet);
		rMoteur.Moteur_Physique.gScene->addCollection(*(objet->collection));

		if (key > 1 and !is_tunnel) {
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
		float z = objet->oi.object.min_z+50;
		porte->place(x, objet->getHeightAt(x, z), z);
		porte->pred = std::bind(&SceneManager::afficherSceneFin, this);

		add(porte);
		collider_pool.push_back(porte);
		return false;
	}

	bool SceneManager::addToZone(int key, StaticObject* objet)
	{
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		rMoteur.mtxAddZone.lock();
		zones[key].emplace_back(objet);
		rMoteur.Moteur_Physique.gScene->addActor(*(objet->body));
		rMoteur.mtxAddZone.unlock();
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
		rMoteur.mtxAddZone.lock();
		zones[key].emplace_back(bonus);
		rMoteur.Moteur_Physique.gScene->addActor(*(bonus->body));
		bonus_pool.push_back(bonus);
		rMoteur.mtxAddZone.unlock();
		return true;
	}


	bool SceneManager::addEntities(EntityManager* pEm)
	{
		
			CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
			
			//Ajout a la scene de rendu
			zones[0].emplace_back(&(pEm->pPlayer->playerCharacter));
			UI.emplace_back(pEm->pPlayer->pParticleManager->pAfficheurParticule);
			//Ajout à la scene Physique
			rMoteur.Moteur_Physique.gScene->addActor(*(pEm->pPlayer->playerCharacter.body));
			for (auto enemy : pEm->enemies) {
				zones[0].emplace_back(&(enemy->enemyCharacter));
				rMoteur.Moteur_Physique.gScene->addActor(*(enemy->enemyCharacter.body));
				UI.emplace_back(enemy->pParticleManager->pAfficheurParticule);
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
		rMoteur.scoreboard.pAfficheurScoreboard->onScreen = false;
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
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		for (auto bonus : bonus_pool) {
			bonus->visible = true;
			bonus->triggered = false;	

			float x=0.0f;
			float y=0.0f;
			float z=0.0f;
			if (bonus->terrain == 3) {
				int pos = (int)(rand() / float(RAND_MAX) * rMoteur.pTerrain3->oi.object.points_.size());


				  x = rMoteur.pTerrain3->oi.object.points_[pos].x;
				  y = rMoteur.pTerrain3->oi.object.points_[pos].y;
				  z = rMoteur.pTerrain3->oi.object.points_[pos].z;

			}
			else if (bonus->terrain == 2) {
				int pos = (int)(rand() / float(RAND_MAX) * rMoteur.pTerrain2->oi.object.points_.size());


				  x = rMoteur.pTerrain2->oi.object.points_[pos].x;
				  y = rMoteur.pTerrain2->oi.object.points_[pos].y;
				  z = rMoteur.pTerrain2->oi.object.points_[pos].z;
			}
			else if (bonus->terrain == 1) {
				int pos = (int)(rand() / float(RAND_MAX) * rMoteur.pTerrain1->oi.object.points_.size());


				x = rMoteur.pTerrain1->oi.object.points_[pos].x;
				y = rMoteur.pTerrain1->oi.object.points_[pos].y;
				z = rMoteur.pTerrain1->oi.object.points_[pos].z;
			}

			bonus->place(x, y + 2.0f, z);
		}

	}

	void SceneManager::resetEntities()
	{
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();

		
		rMoteur.Moteur_Physique.gScene->removeActor(*(rMoteur.pEntityManager->pPlayer->playerCharacter.body));

		for (auto enemy : rMoteur.pEntityManager->enemies) {
			rMoteur.Moteur_Physique.gScene->removeActor(*(enemy->enemyCharacter.body));
		}

	
		zones[0].clear();
		rMoteur.pEntityManager->resetEntities();

	}

	void SceneManager::resetZones()
	{

		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		for (auto bonus : obstacle_pool) {
			

			float x = 0.0f;
			float y = 0.0f;
			float z = 0.0f;
			if (bonus->terrain == 3) {
				int pos = (int)(rand() / float(RAND_MAX) * rMoteur.pTerrain3->oi.object.points_.size());


				x = rMoteur.pTerrain3->oi.object.points_[pos].x;
				y = rMoteur.pTerrain3->oi.object.points_[pos].y;
				z = rMoteur.pTerrain3->oi.object.points_[pos].z;

			}
			else if (bonus->terrain == 2) {
				int pos = (int)(rand() / float(RAND_MAX) * rMoteur.pTerrain2->oi.object.points_.size());


				x = rMoteur.pTerrain2->oi.object.points_[pos].x;
				y = rMoteur.pTerrain2->oi.object.points_[pos].y;
				z = rMoteur.pTerrain2->oi.object.points_[pos].z;
			}
			else if (bonus->terrain == 1) {
				int pos = (int)(rand() / float(RAND_MAX) * rMoteur.pTerrain1->oi.object.points_.size());


				x = rMoteur.pTerrain1->oi.object.points_[pos].x;
				y = rMoteur.pTerrain1->oi.object.points_[pos].y;
				z = rMoteur.pTerrain1->oi.object.points_[pos].z;
			}

			bonus->place(x, y, z);
		}

	}

	vector<std::pair<string,float>> SceneManager::PlusProcheFin() {
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		auto posPlayer = rMoteur.pEntityManager->pPlayer->playerCharacter.body->getGlobalPose().p;
		auto posEnd = collider_pool[collider_pool.size() - 1]->body->getGlobalPose().p;
		auto disPlayer = (posPlayer - posEnd).magnitude();
		vector<std::pair<string, float>> ret;
		 ret.push_back(make_pair(string(rMoteur.pEntityManager->pPlayer->nom),disPlayer)) ;

		for (auto enemy : rMoteur.pEntityManager->enemies) {

			auto posEnemy = enemy->enemyCharacter.body->getGlobalPose().p;
			auto disEnemy = (posEnemy - posEnd).magnitude();
			bool inser = false;
			for (int i = 0; i < ret.size(); i++) {
				if (disEnemy < ret[i].second) {
					ret.insert(ret.begin() + i, make_pair(string(enemy->nom), disEnemy));
					inser = true;
					break;
				}
			}
			if (!inser) {
				ret.push_back(make_pair(string(enemy->nom), disEnemy));
			}
			
		}
		return ret;

	}

	void SceneManager::resetTime()
	{
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		rMoteur.minutePassee = 0;
		rMoteur.secondePassee = 0.0f;
	}

	void SceneManager::resetParty()
	{
		resetZones();
		resetCollider();
		resetBonus();
		resetEntities();
		resetTime();
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
		Obstacle* obs = new Obstacle(rMoteur.pDispositif, obj,zone);
		rMoteur.mtx.lock();
		obs->SetTexture(rMoteur.TexturesManager.GetNewTexture(texture, rMoteur.pDispositif));
		rMoteur.mtx.unlock();
		obs->place(x, y, z);
		addToZone(zone, obs);
		rMoteur.mtxAddZone.lock();
		obstacle_pool.push_back(obs);
		rMoteur.mtxAddZone.unlock();
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
