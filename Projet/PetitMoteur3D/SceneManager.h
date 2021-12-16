#pragma once
#include "ImportedObject.h"
#include <unordered_map>
#include "EntityManager.h"
#include "StaticObject.h"
#include "DynamicObject.h"
#include "Terrain.h"
#include "BasicColider.h"
#include "AfficheurSprite.h"
#include "AfficheurPanneau.h"
#include "Skybox.h"
#include "Obstacle.h"
#include "Bonus.h"
#include <random>
//class EntityManager;
namespace PM3D {

	//
	//   TEMPLATE : SceneManager
	//
	//   BUT : Gere toutes les scenes du jeu
	//
	class SceneManager
	{
	public:
		std::unordered_map<int, std::vector<std::unique_ptr<CObjet3D>>> zones;
		std::unordered_map<int, std::vector<std::unique_ptr<CObjet3D>>> menu;
		bool onMenu = true;
		std::vector<std::unique_ptr<CObjet3D>> UI;

		std::vector<int> zonesActives{};
		std::vector<BasicColider*> collider_pool{};
		std::vector<Bonus*> bonus_pool{};
		mt19937 prng{ random_device{}() };
		std::vector<Obstacle*> obstacle_pool{};

		SceneManager();

		bool createZone(int key);
		bool addToZone(int key, Terrain* objet, bool is_tunnel = false);
		bool addLastZone(int key, Terrain* objet);
		bool addToZone(int key, StaticObject* objet);
		bool addToZone(int key, DynamicObject* objet);
		bool addToZone(Skybox* s);
		bool addToUI(CAfficheurSprite* objet);
		bool addToUI(CAfficheurPanneau* objet);
		bool add(BasicColider* objet);
		bool add(int key, Bonus* bonus);
		bool addEntities(EntityManager* pEm);

		bool afficherSceneFin();

		void resetCollider();
		void resetBonus();
		void resetEntities();
		void resetZones();
		vector<std::pair<string, float>> PlusProcheFin();
		void resetTime();

		void resetParty();

		void desactiverZone(int i);

		void placeRandomObstacle(int zone, Terrain* where, string obj, wstring texture);
		void placeRandomBonus(int zone, Terrain* where, Bonus* obs);
		void placeRandomObstacle(int zone, Terrain* where, Obstacle obs);
	};
};
