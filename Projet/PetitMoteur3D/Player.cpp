#include "stdafx.h"
#include "Player.h"
#include "Sphere.h"
#include "MoteurWindows.h"
namespace PM3D {



	Player::Player(PxVec3 pos, PxVec3 dir, Sphere& object)
		: lastCheckPointPos{ pos }
		, lastCheckPointDir{ dir }
		, playerCharacter{ object }
		, vitesse{ 0 }
		, nom{ "Player" }
	{
		playerCharacter = object;
		playerCharacter.body->setGlobalPose(PxTransform(pos));
		pParticleManager = new ParticleManager(CMoteurWindows::GetInstance().pDispositif, XMFLOAT3(pos.x, pos.y, pos.z),10);
		pParticleManager->AjouterParticle("SnowParticle.dds", 1, 1);
		pParticleManager->AjouterParticle("SnowParticle.dds", 1, 1);
		pParticleManager->AjouterParticle("SnowParticle.dds", 1, 1);
		pParticleManager->AjouterParticle("SnowParticle.dds", 1, 1);
		pParticleManager->AjouterParticle("SnowParticle.dds", 1, 1);
		//pAfficheurParticule->onScreen = true;
	}

	Player::Player(PxVec3 pos, PxVec3 dir, Cube& object)
		: lastCheckPointPos{ pos }
		, lastCheckPointDir{ dir }
		, playerCharacter{ object }
		, vitesse{ 0 }
		, nom{ "Player" }
	{
		playerCharacter = object;
		/*playerCharacter.body->setGlobalPose(PxTransform(pos));
		pAfficheurParticule = new CAfficheurPanneau(CMoteurWindows::GetInstance().pDispositif);
		pAfficheurParticule->AjouterPanneau("SnowParticle.dds", XMFLOAT3(pos.x, pos.y, pos.z), 1, 1);
		pAfficheurParticule->AjouterPanneau("SnowParticle.dds", XMFLOAT3(pos.x, pos.y, pos.z), 1, 1);
		pAfficheurParticule->AjouterPanneau("SnowParticle.dds", XMFLOAT3(pos.x, pos.y, pos.z), 1, 1);
		pAfficheurParticule->AjouterPanneau("SnowParticle.dds", XMFLOAT3(pos.x, pos.y, pos.z), 1, 1);
		pAfficheurParticule->AjouterPanneau("SnowParticle.dds", XMFLOAT3(pos.x, pos.y, pos.z), 1, 1);
		pAfficheurParticule->onScreen = true;*/
	}

	void Player::Anime()
	{
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		CDIManipulateur& rGestionnaireDeSaisie =
			rMoteur.GetGestionnaireDeSaisie();
		auto mEye = playerCharacter.body->getGlobalPose().p;
		auto mDir = playerCharacter.body->getLinearVelocity().getNormalized();
		mDir.normalize();
		auto speed = 20.0f;
		PxVec3 viewY = mDir.cross(PxVec3(0, 1, 0)).getNormalized();

		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_LEFT)) {
			playerCharacter.body->addForce(viewY * speed * playerCharacter.scale, PxForceMode::eIMPULSE);
			//mEye += viewY * speed;
		}
		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_RIGHT)) {
			playerCharacter.body->addForce(-viewY * speed * playerCharacter.scale, PxForceMode::eIMPULSE);
			//mEye -= viewY * speed;
		}

		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_UP)) {
			playerCharacter.body->addForce(mDir * speed * playerCharacter.scale, PxForceMode::eIMPULSE);
		}

		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_R)) {
			Respawn();
		}

		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_ESCAPE) && !rMoteur.sceneManager.onMenu) {
			rMoteur.setMenuOn();
		}

		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_P)) {
			rMoteur.pause = true;
		}
		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_O)) {
			rMoteur.pause = false;
		}


		if (need_to_respawn)
		{
			need_to_respawn = false; 
			Respawn();
		}

		pParticleManager->play();
		playerCharacter.UpdatePhysique(playerCharacter.scale);
		//playerCharacter.body->setGlobalPose(PxTransform(mEye));
		const auto v = playerCharacter.body->getLinearVelocity();
		vitesse = static_cast<float>(sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2)));
	}



	void Player::Crash()
	{
		playerCharacter.scale = playerCharacter.scale / 2;

		playerCharacter.body->setMaxLinearVelocity(12.0f * playerCharacter.scale);
		playerCharacter.body->setMass(100.0f * playerCharacter.scale);

		pParticleManager->UpdatePosition(XMFLOAT3(
			playerCharacter.body->getGlobalPose().p.x,
			playerCharacter.body->getGlobalPose().p.y,
			playerCharacter.body->getGlobalPose().p.z
		));
		/*for (auto& p : pParticleManager->pAfficheurParticule->tabPanneau) {
			p->position = XMFLOAT3(playerCharacter.body->getGlobalPose().p.x, playerCharacter.body->getGlobalPose().p.y, playerCharacter.body->getGlobalPose().p.z);
		}*/
	}

	void Player::Growth()
	{
		playerCharacter.scale += 0.005f;
	}

	void Player::Shrink()
	{
		playerCharacter.scale -= 0.005f;
		if (playerCharacter.scale < 0.1)
		{
			need_to_respawn = true; 
		}
	}

	void Player::Respawn()
	{
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		//playerCharacter.InitPhysique(nom); 
		playerCharacter.body->setGlobalPose(PxTransform(lastCheckPointPos));
		playerCharacter.scale = 1;
		playerCharacter.UpdatePhysique(playerCharacter.scale);
		playerCharacter.body->setMaxLinearVelocity(12.0f * playerCharacter.scale);
		playerCharacter.body->setMass(100.0f * playerCharacter.scale);

	}

}
