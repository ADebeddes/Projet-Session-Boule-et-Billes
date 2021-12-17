#include "stdafx.h"
#include "Enemy.h"
#include "MoteurWindows.h"
namespace PM3D {

	Enemy::Enemy(PxVec3 pos, DynamicObject& object, const char* nom)
		: lastCheckPointPos{ pos }
		, enemyCharacter{ object }
		, nom{ nom } {
		enemyCharacter = object; 

		enemyCharacter.body->setGlobalPose(PxTransform(pos));
		pParticleManager = new ParticleManager(CMoteurWindows::GetInstance().pDispositif,XMFLOAT3(pos.x, pos.y, pos.z), 20);
		pParticleManager->AjouterParticle("SnowParticle.dds", 1, 1);
		pParticleManager->AjouterParticle("SnowParticle.dds", 1, 1);
		pParticleManager->AjouterParticle("SnowParticle.dds", 1, 1);
		pParticleManager->AjouterParticle("SnowParticle.dds", 1, 1);
		pParticleManager->AjouterParticle("SnowParticle.dds", 1, 1);
	}
	Enemy::~Enemy() {
		free(pParticleManager);
	}
	void Enemy::Anime()
	{
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();

		enemyCharacter.UpdatePhysique(enemyCharacter.scale); 
		if (need_to_respawn) {
			Respawn();
		}

		pParticleManager->play();
	}


	void Enemy::Crash()
	{
		enemyCharacter.scale = enemyCharacter.scale / 2;

		enemyCharacter.body->setMaxLinearVelocity(12.0f * enemyCharacter.scale);
		enemyCharacter.body->setMass(100.0f * enemyCharacter.scale);

		pParticleManager->UpdatePosition(XMFLOAT3(
			enemyCharacter.body->getGlobalPose().p.x, 
			enemyCharacter.body->getGlobalPose().p.y,
			enemyCharacter.body->getGlobalPose().p.z
		));
	}

	void Enemy::Growth()
	{
		enemyCharacter.scale += 0.005f;
	}
	void Enemy::Shrink()
	{
		enemyCharacter.scale -= 0.005f;
		if (enemyCharacter.scale < 0.1)
		{
			need_to_respawn = true;
		}
	}

	void Enemy::Respawn()
	{
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		enemyCharacter.scale = 1;
		enemyCharacter.body->setGlobalPose(PxTransform(lastCheckPointPos));
		enemyCharacter.UpdatePhysique(enemyCharacter.scale);
		enemyCharacter.body->setMaxLinearVelocity(12.0f * enemyCharacter.scale);
		enemyCharacter.body->setMass(100.0f * enemyCharacter.scale);
		enemyCharacter.body->setLinearVelocity(PxVec3(0.0f, 0.0f, 0.0f));
		need_to_respawn = false;
	}

}