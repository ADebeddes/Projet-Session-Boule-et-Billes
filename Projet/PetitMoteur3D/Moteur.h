#pragma once
#ifndef Moteur

#include "Physique.h"
#endif // !Moteur


#include <thread>
#include "Singleton.h"
#include "Skybox.h"
#include <functional>
#include <map>
#include <utility>
#include "dispositif.h"

#include "Objet3D.h"
#include "Bloc.h"
#include "BlocEffet1.h"
#include "Terrain.h"
#include "Obstacle.h"
#include "Cube.h"
#include "Sphere.h"
#include "EntityManager.h"
#include "CDIManipulateur.h"
#include "GestionnaireDeTextures.h"

#include "Camera.h"

#include "SceneManager.h"
#include "CameraManager.h"
#include "MenuController.h"

#include "AfficheurSprite.h"
#include "AfficheurTexte.h"
#include "AfficheurPanneau.h"

#include "Bonus.h"
#include "NosBonus.h"

#include "LOD.h"
#include "PanneauPE.h"

#include <string>
#include <cstdlib> 
#include <ctime> 
#include <future>
#include <mutex> 

#include "SoundClass.h"

#include "Scoreboard.h"

#define PX_RELEASE(x)if(x) { x->release();x =NULL; }


using namespace physx;
using namespace std;



namespace PM3D
{

	const int IMAGESPARSECONDE = 60;
	const double EcartTemps = 1.0 / static_cast<double>(IMAGESPARSECONDE);

	enum BonusTypes { UP, DOWN, LIGHTNING };

	const int nb_sapin = 300; 

	

	//
	//   TEMPLATE : CMoteur
	//
	//   BUT : Template servant a construire un objet Moteur qui implantera les
	//         aspects "generiques" du moteur de jeu
	//
	//   COMMENTAIRES :
	//
	//        Comme plusieurs de nos objets representeront des elements uniques 
	//		  du systeme (ex: le moteur lui-meme, le lien vers 
	//        le dispositif Direct3D), l'utilisation d'un singleton 
	//        nous simplifiera plusieurs aspects.
	//
	template <class T, class TClasseDispositif> class CMoteur :public CSingleton<T>
	{
	public:
		Physique Moteur_Physique{};
		std::mutex mtx;
		std::mutex mtxAddZone;
	public:

		virtual void Run()
		{
			bool bBoucle = true;

			while (bBoucle)
			{
				// Propre a la plateforme - (Conditions d'arret, interface, messages)
				bBoucle = RunSpecific();

				// appeler la fonction d'animation
				if (bBoucle)
				{
					bBoucle = Animation();
				}
			}
		}

		virtual int Initialisations()
		{
			// Propre a la plateforme
			InitialisationsSpecific();

			// * Initialisation du dispositif de rendu
			pDispositif = CreationDispositifSpecific(CDS_FENETRE);

			// * Initialisation de la scene
			InitScene();

			// * Initialisation des parametres de l'animation et 
			//   preparation de la premiere image
			InitAnimation();

			return 0;
		}

		virtual bool Animation()
		{
			// methode pour lire l'heure et calculer le 
			// temps ecoule
			const int64_t TempsCompteurCourant = GetTimeSpecific();
			const double TempsEcoule = GetTimeIntervalsInSec(TempsCompteurPrecedent, TempsCompteurCourant);

			// Est-il temps de rendre l'image?
			if (TempsEcoule > EcartTemps)
			{
				// Affichage optimise
				pDispositif->Present(); // On enlevera plus tard

				// On prepare la prochaine image
				AnimeScene(static_cast<float>(TempsEcoule));

				// On rend l'image sur la surface de travail
				// (tampon d'arriere plan)
				RenderScene();

				// Calcul du temps du prochain affichage
				TempsCompteurPrecedent = TempsCompteurCourant;
			}

			return true;
		}
		void setMenuOn() {
			sceneManager.onMenu = true;
			pAfficheurSprite->onScreen = false;
			pAfficheurPanneau->onScreen = false;
			scoreboard.pAfficheurScoreboard->onScreen = false;
			menuController->setOn();
			m_Sound->PlayWaveFile(m_Sound->MusiqueMenuPrincipal);
			m_Sound->fadeIn1();
			m_Sound->fadeOut2();
		}
		void setMenuOff() {
			sceneManager.onMenu = false;
			pAfficheurSprite->onScreen = true;
			pAfficheurPanneau->onScreen = true;
			menuController->setOff();
			m_Sound->PlayWaveFile(m_Sound->GameMusic);
			m_Sound->fadeIn2();
			m_Sound->fadeOut1();

		}
		void updateRatioEcran() {
			const float ratioDAspect = static_cast<float>(pDispositif->GetLargeur()) / static_cast<float>(pDispositif->GetHauteur());
			const float planRapproche = 0.1f;
			const float planEloigne = 20000.0f;
			m_MatProj.r[1].vector4_f32[1] = ratioDAspect;
			m_MatViewProj = m_MatView * m_MatProj;
			camManager.setProjection(90.0f, ratioDAspect, planRapproche, planEloigne);
		}
		const XMMATRIX& GetMatView() const { return m_MatView; }
		const XMMATRIX& GetMatProj() const { return m_MatProj; }
		const XMMATRIX& GetMatViewProj() const { return m_MatViewProj; }

		CGestionnaireDeTextures& GetTextureManager() { return TexturesManager; }

	protected:
		~CMoteur()
		{
			Cleanup();
		}

		// Specifiques - Doivent etre implantes
		virtual bool RunSpecific() = 0;
		virtual int InitialisationsSpecific() = 0;

		virtual int64_t GetTimeSpecific() const = 0;
		virtual double GetTimeIntervalsInSec(int64_t start, int64_t stop) const = 0;

		virtual TClasseDispositif* CreationDispositifSpecific(const CDS_MODE cdsMode) = 0;
		virtual void BeginRenderSceneSpecific() = 0;
		virtual void EndRenderSceneSpecific() = 0;

		// Autres fonctions
		virtual int InitAnimation()
		{
			TempsSuivant = GetTimeSpecific();
			TempsCompteurPrecedent = TempsSuivant;

			// premi�re Image
			RenderScene();

			return true;
		}

		// Fonctions de rendu et de pr�sentation de la sc�ne
		virtual bool RenderScene()
		{
			BeginRenderSceneSpecific();

			pPanneauPE->DebutPostEffect();
			// Scene sur surface de rendu POST-EFFECT
			BeginRenderSceneSpecific();


			//Nous rendons uniquements les zones actives
			for (auto i : sceneManager.zonesActives) {
				// Appeler les fonctions de dessin de chaque objet de la sc�ne
				for (auto& object3D : sceneManager.zones[i])
				{
					if (object3D->visible && LOD::afficherFaceCamera(camManager.getActive(), object3D->pos, ( int)(2000- (nb_sapin* 4)))) {
						object3D->Draw();
					}
				}
			}

			for (auto& object3D : sceneManager.UI)
			{
				object3D->Draw();
			}

			if (scoreboard.init) {
				scoreboard.pAfficheurScoreboard->Draw();
			}

			EndRenderSceneSpecific();
			pPanneauPE->FinPostEffect();
			pPanneauPE->Draw();

			EndRenderSceneSpecific();
			return true;
		}

		virtual void Cleanup()
		{
			// d�truire les objets
			
			sceneManager.zones.clear();
			sceneManager.obstacle_pool.clear();
			sceneManager.bonus_pool.clear();

			pEntityManager->enemies.clear();
			free(pEntityManager->pPlayer);
			free(pEntityManager);

			// D�truire le dispositif
			if (pDispositif)
			{
				delete pDispositif;
				pDispositif = nullptr;
			}
			Moteur_Physique.cleanupPhysics(true);
		}

		virtual int InitScene()
		{
			// Initialisation du moteur Physique 
			Moteur_Physique.initPhysics(true);

			// Initialisation des matrices View et Proj
			// Dans notre cas, ces matrices sont fixes
			m_MatView = XMMatrixLookAtLH(XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f),
				XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f),
				XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f));
			const float ratioDAspect = static_cast<float>(pDispositif->GetLargeur()) / static_cast<float>(pDispositif->GetHauteur());
			const float planRapproche = 0.1f;
			const float planEloigne = 20000.0f;

			m_MatProj = XMMatrixPerspectiveFovLH(
				XM_PIDIV4,
				ratioDAspect,
				planRapproche,
				planEloigne);

			camManager.setProjection(90.0f, ratioDAspect, planRapproche, planEloigne);

			m_MatViewProj = m_MatView * m_MatProj;

			// Initialisation des objets 3D - cr�ation et/ou chargement
			if (!InitObjets())
			{
				return 1;
			}

			return 0;
		}

		void createRandomObstacle(int numberof, int zone, Terrain* whereTo, std::map<string, wstring> list)
		{
			
				for (const auto obj : list)
				{

					for (int i = 0; i < numberof; i++)
					{
					
						sceneManager.placeRandomObstacle(zone, whereTo, obj.first, obj.second);
						
					}

				}
			
		}

		void createRandomBonus(int numberof, int zone, Terrain* whereTo, BonusTypes  type)
		{
			for (int i = 0; i < numberof; i++)
			{
				Bonus* b;

				switch (type)
				{
				case BonusTypes::UP:
				{
					b = new AgrandirBalle(pDispositif, "boule.obj",zone);
					break;
				}
				case BonusTypes::DOWN:
				{
					b = new DiminuerBalle(pDispositif, "boule.obj", zone);
					break;
				}

				case BonusTypes::LIGHTNING:
				{
					b = new DiminuerAutreBalle(pDispositif, "boule.obj", zone);
					break;
				}
				default:
				{
					// but how ?
					break;
				}
				}
				sceneManager.placeRandomBonus(zone, whereTo, b);
			}
		}





		bool InitObjets()
		{
			objToTextTree.emplace("Tree1.obj", L"fir_tree.dds");
			objToTextTree.emplace("Tree2.obj", L"fir_tree.dds");
			objToTextTree.emplace("Tree3.obj", L"fir_tree.dds");
			objToTextStone.emplace("Rock1.obj", L"RockTexture.dds");
			objToTextStone.emplace("Rock2.obj", L"RockTexture.dds");
			objToTextStone.emplace("Rock3.obj", L"RockTexture.dds");



			////Creation du terrain
			pTerrain1 = new Terrain(pDispositif, "Pente1.obj");

			////Affectation des textures a notre terrain
			pTerrain1->SetTexture(TexturesManager.GetNewTexture(L"snow.dds", pDispositif));
			pTerrain1->SetTexture(TexturesManager.GetNewTexture(L"delimitation.dds", pDispositif));
			pTerrain1->SetTexture(TexturesManager.GetNewTexture(L"poudreuse.dds", pDispositif));

			////// Affectation du filtre a notre terrain
			pTerrain1->SetFilter(TexturesManager.GetNewTexture(L"tundra_road_filter.dds", pDispositif));

			//Rajoute une zone à notre SceneManager
			sceneManager.createZone(1);

			//Rajoute notre terrain à la scene
			sceneManager.addToZone(1, pTerrain1);


			////Creation du terrain
			pTerrain2 = new Terrain(pDispositif, "Pente2.obj");

			////Affectation des textures a notre terrain
			pTerrain2->SetTexture(TexturesManager.GetNewTexture(L"snow.dds", pDispositif));
			pTerrain2->SetTexture(TexturesManager.GetNewTexture(L"delimitation.dds", pDispositif));
			pTerrain2->SetTexture(TexturesManager.GetNewTexture(L"poudreuse.dds", pDispositif));

			////// Affectation du filtre a notre terrain
			pTerrain2->SetFilter(TexturesManager.GetNewTexture(L"tundra_road_filter.dds", pDispositif));

			//Rajoute une zone à notre SceneManager
			sceneManager.createZone(2);

			//Rajoute notre terrain à la scene
			sceneManager.addToZone(2, pTerrain2);

			////Creation du terrain
			pTerrain3 = new Terrain(pDispositif, "Pente4.obj");

			////Affectation des textures a notre terrain
			pTerrain3->SetTexture(TexturesManager.GetNewTexture(L"snow.dds", pDispositif));
			pTerrain3->SetTexture(TexturesManager.GetNewTexture(L"delimitation.dds", pDispositif));
			pTerrain3->SetTexture(TexturesManager.GetNewTexture(L"poudreuse.dds", pDispositif));

			////// Affectation du filtre a notre terrain
			pTerrain3->SetFilter(TexturesManager.GetNewTexture(L"tundra_road_filter.dds", pDispositif));

			//Rajoute une zone à notre SceneManager
			sceneManager.createZone(3);

			sceneManager.addLastZone(3, pTerrain3);

			pTunnel1 = new Terrain(pDispositif, "Tunnel2.obj");

			////Affectation des textures a notre terrain
			pTunnel1->SetTexture(TexturesManager.GetNewTexture(L"granite.dds", pDispositif));

			////// Affectation du filtre a notre terrain
			pTunnel1->SetFilter(TexturesManager.GetNewTexture(L"tundra_road_filter.dds", pDispositif));

			pTunnel2 = new Terrain(pDispositif, "Tunnel2Invers.obj");

			////Affectation des textures a notre terrain
			pTunnel2->SetTexture(TexturesManager.GetNewTexture(L"granite.dds", pDispositif));

			////// Affectation du filtre a notre terrain
			pTunnel2->SetFilter(TexturesManager.GetNewTexture(L"tundra_road_filter.dds", pDispositif));

			//Rajoute notre terrain à la scene
			sceneManager.addToZone(2, pTunnel1, true);
			sceneManager.addToZone(2, pTunnel2, true);

			{
				auto handle1 = std::async(std::launch::async, [&]() {
					createZone1();
					});
				auto handle2 = std::async(std::launch::async, [&]() {
					createZone2();
					});
				auto handle3 = std::async(std::launch::async, [&]() {
					createZone3();
					});
				auto handle4 = std::async(std::launch::async, [&]() {
					createZone1Bonus();
					createZone2Bonus();
					createZone3Bonus();
					});
			};

			nbEnemies = 5;
			pEntityManager = new EntityManager(nbEnemies, PxVec3(-50, pTerrain1->getHeightAt(-50, -60) + 10, -60), pDispositif);

			pAfficheurSprite = new CAfficheurSprite(pDispositif);
			CAfficheurTexte::Init();
			/*const Gdiplus::FontFamily oFamily(L"Arial", nullptr);
			pPolice1 = std::make_unique<Gdiplus::Font>(&oFamily, 16.0f, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
			pTexte1 = new CAfficheurTexte(pDispositif, 128, 24, pPolice1.get());*/
			const Gdiplus::FontFamily oFamily(L"Arial", nullptr);
			pPolice = std::make_unique<Gdiplus::Font>(&oFamily, 36.0f, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
			pTexte1 = new CAfficheurTexte(pDispositif, 135, 40, pPolice.get());

			pPanneauPE = std::make_unique<CPanneauPE>(pDispositif);

			menuController = new MenuController(pDispositif);

			auto e_largeur = pDispositif->GetLargeur();
			auto e_Hauteur = pDispositif->GetHauteur();

			//pAfficheurSprite->AjouterSpriteTexte(pTexte1->GetTextureView(), e_largeur-128, e_Hauteur- 24 );
			pAfficheurSprite->AjouterSpriteTexte(pTexte1->GetTextureView(),"speed", e_largeur - 120, e_Hauteur - 90);
			pAfficheurSprite->AjouterSprite("CompteurBleu.dds", e_largeur -128, e_Hauteur-24, e_largeur / 4, e_Hauteur / 4);
			
			pPolice2 = std::make_unique<Gdiplus::Font>(&oFamily, 36.0f, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
			pTexte2 = new CAfficheurTexte(pDispositif, 110, 36, pPolice2.get(),true);
			pAfficheurSprite->AjouterSpriteTexte(pTexte2->GetTextureView(), "temps", e_largeur/2+50, 67);
			pAfficheurSprite->AjouterSprite("chrono.dds", e_largeur / 2- 100, 100, e_largeur / 8, e_Hauteur / 8);

			pAfficheurPanneau = new CAfficheurPanneau(pDispositif);
			//pAfficheurPanneau->AjouterPanneau("panic.dds",XMFLOAT3(0,0,0),100,100);
			pAfficheurPanneau->AjouterPanneau("Finish.dds", XMFLOAT3(20,-275,pTerrain3->oi.object.min_z+50), 220, 25, true);


			sceneManager.addToUI(pAfficheurSprite);
			sceneManager.addToUI(pAfficheurPanneau);
			sceneManager.addToUI(menuController->AfficheurMenuPrincipal);
			sceneManager.addToUI(menuController->AfficheurOption);
			sceneManager.addToUI(menuController->AfficheurVictoire);
		

			/*BasicColider* porte = new BasicColider(200, 200, 1); 
			porte->place(-50, pTerrain1->getHeightAt(-50, -60) + 10, -70);

			sceneManager.add(porte);*/

			
			PremierePosition = PxVec3(-50, pTerrain1->getHeightAt(-50, -60) + 10, -60);


			/*sceneManager.addEntities(pEntityManager);*/
			sceneManager.addToZone(new Skybox(pDispositif, "Boule.obj"));
			m_Sound->fadeIn1();
			m_Sound->PlayWaveFile(m_Sound->MusiqueMenuPrincipal);
			return true;
		}

		bool AnimeScene(float tempsEcoule)
		{

			if (!sceneManager.onMenu && !pause) {
				{
					Moteur_Physique.stepPhysics(true);
					secondePassee += tempsEcoule;
					if (secondePassee > 60) {
						minutePassee++;
						secondePassee = fmod(secondePassee, 60.0f);
					}
				}

			}
			// Prendre en note le statut du clavier 
			GestionnaireDeSaisie.StatutClavier();

			// Prendre en note l��tat de la souris 
			GestionnaireDeSaisie.SaisirEtatSouris();



			for (auto i : sceneManager.zonesActives) {
				// Appeler les fonctions d'animation
				for (auto& object3D : sceneManager.zones[i])
				{
					object3D->Anime(tempsEcoule);
				}
			}

			camManager.switchCamera();
			camManager.Anime(pEntityManager->pPlayer->playerCharacter, pTerrain1->getHeightAt(camManager.getActive().pos.x, camManager.getActive().pos.z));
			//pEntityManager->pPlayer->Anime();
			pEntityManager->AnimeEntities();
			m_MatViewProj = camManager.getActive().pMatView * camManager.getActive().pMatProj;

			if (GestionnaireDeSaisie.click) {
				if (!menuController->doAction(HoveredOption)) {
					setMenuOff();
				}
				GestionnaireDeSaisie.click = false;
			}


			//Vitesse
			string speed = std::to_string(static_cast<int>(pEntityManager->pPlayer->playerCharacter.body->getLinearVelocity().magnitude())) + " m/s ";
			
			if (static_cast<int>(pEntityManager->pPlayer->playerCharacter.body->getLinearVelocity().magnitude()) < 10) {
				speed = " "s + speed;
			}

			std::wstring w_speed(speed.begin(), speed.end());

			/*string position = "x : " + std::to_string(camManager.getActive().position.vector4_f32[0]) +
				" , y : " + std::to_string(camManager.getActive().position.vector4_f32[1]) +
				" , z : " + std::to_string(camManager.getActive().position.vector4_f32[2]);
			std::wstring w_position(position.begin(), position.end());*/

			
			pTexte1->Ecrire(w_speed);

			//Temps
			string temps = std::to_string(minutePassee) + ":" + std::to_string(static_cast<int>(secondePassee));
			if (minutePassee < 10) {
				 temps = std::to_string(0) + std::to_string(minutePassee) + ":" + std::to_string(static_cast<int>(secondePassee));
				if (secondePassee < 10) {
					 temps = std::to_string(0) + std::to_string(minutePassee) + ":" + std::to_string(0) + std::to_string(static_cast<int>(secondePassee));
				}
				
			}
			
			std::wstring w_temps(temps.begin(), temps.end());
			pTexte2->Ecrire(w_temps);



			if (m_Sound->musiqueFadeOut1) {
				m_Sound->DiminuerSon1(m_Sound->MusiqueMenuPrincipal, &m_Sound->volume1, &m_Sound->musiqueFadeOut1);
			}

			if (m_Sound->musiqueFadeOut2) {
				m_Sound->DiminuerSon2(m_Sound->GameMusic, &m_Sound->volume2, &m_Sound->musiqueFadeOut2);
			}

			if (m_Sound->musiqueFadeIn1) {
				m_Sound->AugmenterSon1(m_Sound->MusiqueMenuPrincipal, &m_Sound->volume1, &m_Sound->musiqueFadeIn1);
			}

			if (m_Sound->musiqueFadeIn2) {
				m_Sound->AugmenterSon2(m_Sound->GameMusic, &m_Sound->volume2, &m_Sound->musiqueFadeIn2);
			}

			if (scoreboard.init) {
				scoreboard.pAfficheurScoreboard->Draw();
			}

			if (scoreboard.init) {
				scoreboard.Anime();
			}

			return true;
		}

	protected:
		// Variables pour le temps de l'animation
		int64_t TempsSuivant;
		int64_t TempsCompteurPrecedent;

		// Les matrices
		XMMATRIX m_MatView;
		XMMATRIX m_MatProj;
		XMMATRIX m_MatViewProj;



		CDIManipulateur GestionnaireDeSaisie;

		// Les managers
		int nbEnemies;
		

		std::map<string, wstring> objToTextTree{};
		std::map<string, wstring> objToTextStone{};

		CAfficheurTexte* pTexte1;
		CAfficheurTexte* pTexte2;

		std::wstring str;

		std::unique_ptr<Gdiplus::Font> pPolice;
		std::unique_ptr<Gdiplus::Font> pPolice2;


	public:
		void createZone3()
		{
			

			createRandomObstacle((int)(nb_sapin*1.2), 3, pTerrain3, objToTextTree);
			createRandomObstacle(20, 3, pTerrain3, objToTextStone);

		}
		void createZone2()
		{

			createRandomObstacle((int)(nb_sapin), 2, pTerrain2, objToTextTree);
			createRandomObstacle(20, 2, pTerrain2, objToTextStone);
		}
		void createZone1()
		{

			createRandomObstacle((int)(nb_sapin*0.8), 1, pTerrain1, objToTextTree);
			createRandomObstacle(20, 1, pTerrain1, objToTextStone);

		}

		void createZone3Bonus()
		{

			createRandomBonus(10, 3, pTerrain3, BonusTypes::UP);
			createRandomBonus(10, 3, pTerrain3, BonusTypes::DOWN);
			createRandomBonus(10, 3, pTerrain3, BonusTypes::LIGHTNING);

		}

		void createZone2Bonus()
		{

			createRandomBonus(10, 2, pTerrain2, BonusTypes::UP);
			createRandomBonus(10, 2, pTerrain2, BonusTypes::DOWN);
			createRandomBonus(10, 2, pTerrain2, BonusTypes::LIGHTNING);

		}

		void createZone1Bonus()
		{

			createRandomBonus(10, 1, pTerrain1, BonusTypes::UP);
			createRandomBonus(10, 1, pTerrain1, BonusTypes::DOWN);
			createRandomBonus(10, 1, pTerrain1, BonusTypes::LIGHTNING);

		}


		Terrain* pTerrain1;
		Terrain* pTerrain2;
		Terrain* pTerrain3;
		Terrain* pTunnel1;
		Terrain* pTunnel2;
		Scoreboard scoreboard;
		SoundClass* m_Sound;
		string HoveredOption;
		CameraManager camManager;
		bool pause = false;
		// Le gestionnaire de texture
		CGestionnaireDeTextures TexturesManager;
		// Le dispositif de rendu
		CAfficheurSprite* pAfficheurSprite;
		CAfficheurPanneau* pAfficheurPanneau;
		TClasseDispositif* pDispositif;
		PxVec3 PremierePosition;
		MenuController* menuController;
		SceneManager sceneManager;
		EntityManager* pEntityManager;
		std::unique_ptr<CPanneauPE> pPanneauPE;
		CDIManipulateur& GetGestionnaireDeSaisie() { return GestionnaireDeSaisie; }
		int minutePassee;
		float secondePassee;
	};

} // namespace PM3D
