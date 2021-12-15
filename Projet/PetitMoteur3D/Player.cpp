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
	}

	Player::Player(PxVec3 pos, PxVec3 dir, Cube& object)
		: lastCheckPointPos{ pos }
		, lastCheckPointDir{ dir }
		, playerCharacter{ object }
		, vitesse{ 0 }
		, nom{ "Player" }
	{
		playerCharacter = object;
		playerCharacter.body->setGlobalPose(PxTransform(pos));
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

		playerCharacter.UpdatePhysique(playerCharacter.scale);
		//playerCharacter.body->setGlobalPose(PxTransform(mEye));
		const auto v = playerCharacter.body->getLinearVelocity();
		vitesse = static_cast<float>(sqrt(pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2)));
	}



	void Player::Crash()
	{
		playerCharacter.scale = playerCharacter.scale / 2;

		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		rMoteur.m_Sound->PlayWaveFile(rMoteur.m_Sound->Crash);
		playerCharacter.body->setMaxLinearVelocity(12.0f * playerCharacter.scale);
		playerCharacter.body->setMass(100.0f * playerCharacter.scale);

	}

	void Player::Growth()
	{
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		playerCharacter.scale += 0.005f;
		if (!rMoteur.m_Sound->growthPlayed) {
			rMoteur.m_Sound->growthPlayed = true;
			rMoteur.m_Sound->PlayWaveFile(rMoteur.m_Sound->Growth);
		}
		
	
	}

	void Player::Shrink()
	{

		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		if (rMoteur.m_Sound->growthPlayed) {
			rMoteur.m_Sound->growthPlayed = false;
			rMoteur.m_Sound->stopWaveFile(rMoteur.m_Sound->Growth);
		}
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
		playerCharacter.body->setLinearVelocity(PxVec3(0.0f, 0.0f, 0.0f));
		rMoteur.m_Sound->PlayWaveFile(rMoteur.m_Sound->Respawn);

	}

}
