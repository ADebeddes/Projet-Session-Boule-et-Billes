#include "stdafx.h"
#include "Cube.h"
#include "resource.h"
#include "MoteurWindows.h"

namespace PM3D

{
	Cube::Cube(CDispositifD3D11* pDispositif_, string path, char* name)
		: DynamicObject(pDispositif_, path)
		
	{
		object_name = name; 
		InitPhysique(object_name);
	}


	void Cube::Anime(float tempsEcoule)
	{
		tempsEcoule;

		auto tm = body->getGlobalPose();
		matWorld = XMMatrixScaling(scale, scale, scale);
		matWorld = matWorld * XMMatrixRotationQuaternion(XMVectorSet(tm.q.x, tm.q.y, tm.q.z, tm.q.w));
		matWorld = matWorld * XMMatrixTranslation(tm.p.x, tm.p.y, tm.p.z);

		pos = XMFLOAT3(tm.p.x, tm.p.y, tm.p.z);
		if (tm.p.y < -1000.0f)
		{
			body->setGlobalPose(PxTransform(PxVec3(150, 130, 0)));
		}
	}


	void Cube::InitPhysique(char* name)
	{
		object_name = name;
		
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();

		PxShape* shape = rMoteur.Moteur_Physique.gPhysics->createShape(PxBoxGeometry(1, 1, 1), *rMoteur.Moteur_Physique.gMaterial, true);
		const PxTransform localTm = PxTransform(PxVec3(0, 0, 0));
		PxFilterData filterData;
		filterData.word0 = FilterGroup::boule;
		filterData.word1 = FilterGroup::boule || FilterGroup::obstacle || FilterGroup::terrain;
		shape->setSimulationFilterData(filterData);
		shape->setName(object_name);

		body = rMoteur.Moteur_Physique.gPhysics->createRigidDynamic(localTm);
		body->attachShape(*shape);
		body->setMass(100.0f);
		PxRigidBodyExt::setMassAndUpdateInertia(*body, 100.0f, 0, false);
		body->setContactReportThreshold(PxReal{ 200000.0f });

	}
	void Cube::UpdatePhysique(float scale)
	{
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();

		const PxU32 numShapes = body->getNbShapes();
		PxShape** shapes = (PxShape**)malloc(sizeof(PxShape*) * numShapes);
		body->getShapes(shapes, numShapes);

		for (PxU32 i = 0; i < numShapes; i++)
		{
			PxShape* shape = shapes[i];
			body->detachShape(*shape);
		}
		free(shapes);

		PxShape* shape = rMoteur.Moteur_Physique.gPhysics->createShape(PxBoxGeometry(1*scale, 1 * scale, 1 * scale), *rMoteur.Moteur_Physique.gMaterial, true);

		PxFilterData filterData;
		filterData.word0 = FilterGroup::boule;
		filterData.word1 = FilterGroup::boule || FilterGroup::obstacle || FilterGroup::terrain;
		shape->setSimulationFilterData(filterData);
		shape->setName(object_name);
		body->attachShape(*shape);

		body->setMaxLinearVelocity(12.0f * scale);
		body->setMass(100.0f * scale);
	}
}

