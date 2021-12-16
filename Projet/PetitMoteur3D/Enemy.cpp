#include "stdafx.h"
#include "Enemy.h"
#include "MoteurWindows.h"
namespace PM3D {

	//Enemy::Enemy(PxVec3 pos, DynamicObject& object,const char* nom) 
	//	: lastCheckPointPos{ pos }
	//	, enemyCharacter{ object }
	//	, nom{ nom } {
	//	//CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
	//	enemyCharacter.body->setGlobalPose(PxTransform(pos));
	//	const PxU32 numShapes = enemyCharacter.body->getNbShapes();
	//	PxFilterData filterData;
	//	filterData.word0 = FilterGroup::boule;
	//	filterData.word1 = FilterGroup::boule || FilterGroup::obstacle || FilterGroup::terrain;
	//	PxShape** shapes = (PxShape**)malloc(sizeof(PxShape*) * numShapes);
	//	enemyCharacter.body->getShapes(shapes, numShapes);

	//	for (PxU32 i = 0; i < numShapes; i++)
	//	{
	//		PxShape* shape = shapes[i];
	//		shape->setSimulationFilterData(filterData);
	//		shape->setName(nom);
	//	}
	//	free(shapes);
	//	enemyCharacter.body->setContactReportThreshold(PxReal(400000.0f));

	//}

	Enemy::Enemy(PxVec3 pos, DynamicObject& object, const char* nom)
		: lastCheckPointPos{ pos }
		, enemyCharacter{ object }
		, nom{ nom } {
		//CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		enemyCharacter = object; 

		enemyCharacter.body->setGlobalPose(PxTransform(pos));
		pParticleManager = new ParticleManager(CMoteurWindows::GetInstance().pDispositif,XMFLOAT3(pos.x, pos.y, pos.z), 20);
		pParticleManager->AjouterParticle("SnowParticle.dds", 1, 1);
		pParticleManager->AjouterParticle("SnowParticle.dds", 1, 1);
		pParticleManager->AjouterParticle("SnowParticle.dds", 1, 1);
		pParticleManager->AjouterParticle("SnowParticle.dds", 1, 1);
		pParticleManager->AjouterParticle("SnowParticle.dds", 1, 1);
		//pAfficheurParticule->onScreen = true;
	}

	//void Enemy::Anime()
	//{
	//	CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
	//	

	//	/*if (enemyCharacter.body->getGlobalPose().p.y < -200) {
	//		Respawn();
	//	}*/

	//	const PxU32 numShapes = enemyCharacter.body->getNbShapes();
	//	PxShape** shapes = (PxShape**)malloc(sizeof(PxShape*) * numShapes);
	//	enemyCharacter.body->getShapes(shapes, numShapes);

	//	for (PxU32 i = 0; i < numShapes; i++)
	//	{
	//		PxShape* shape = shapes[i];
	//		enemyCharacter.body->detachShape(*shape);
	//	}
	//	free(shapes);


	//	PxShape* shape = rMoteur.Moteur_Physique.gPhysics->createShape(PxSphereGeometry(0.8f * enemyCharacter.scale), *rMoteur.Moteur_Physique.gMaterial, true);
	//	PxFilterData filterData;
	//	filterData.word0 = FilterGroup::boule;
	//	filterData.word1 = FilterGroup::boule || FilterGroup::obstacle || FilterGroup::terrain;
	//	shape->setSimulationFilterData(filterData);
	//	shape->setName(nom);
	//	enemyCharacter.body->attachShape(*shape);

	//	enemyCharacter.body->setMaxLinearVelocity(12.0f * enemyCharacter.scale);
	//	enemyCharacter.body->setMass(100.0f * enemyCharacter.scale);
	//}

	void Enemy::Anime()
	{
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();

		enemyCharacter.UpdatePhysique(enemyCharacter.scale); 

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
		/*for (auto& p : pParticleManager->pAfficheurParticule->tabPanneau) {
			p->position = XMFLOAT3(enemyCharacter.body->getGlobalPose().p.x, enemyCharacter.body->getGlobalPose().p.y, enemyCharacter.body->getGlobalPose().p.z);
		}*/
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
			Respawn(); 
		}
	}

	void Enemy::Respawn()
	{
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		PxShape* shape = rMoteur.Moteur_Physique.gPhysics->createShape(PxSphereGeometry(0.8f * enemyCharacter.scale), *rMoteur.Moteur_Physique.gMaterial, true);

		enemyCharacter.body->attachShape(*shape);
		enemyCharacter.body->setGlobalPose(PxTransform(lastCheckPointPos));
		enemyCharacter.scale = 1;

		enemyCharacter.body->setMaxLinearVelocity(12.0f * enemyCharacter.scale);
		enemyCharacter.body->setMass(100.0f * enemyCharacter.scale);
	}

}