#include "stdafx.h"
#include "BasicColider.h"
#include "resource.h"
#include "MoteurWindows.h"
namespace PM3D
{

	int BasicColider::count = 1; 


	BasicColider::BasicColider(int x, int y, int z )
	{
		number = count; 
		count++; 
		position = PxVec3(x*1.0f, y*1.0f, z*1.0f);
		initPhysique(x*1.0f, y * 1.0f, z * 1.0f);
	}

	bool BasicColider::initPhysique(float x, float y, float z )
	{
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();

		PxShape* shape = rMoteur.Moteur_Physique.gPhysics->createShape(PxBoxGeometry(x, y, z), *rMoteur.Moteur_Physique.gMaterial, true);
		PxTransform localTm(PxVec3(0, 0, 0));

		body = rMoteur.Moteur_Physique.gPhysics->createRigidStatic(localTm);
		body->attachShape(*shape);

		PxFilterData filterData;
		filterData.word0 = FilterGroup::colider;
		filterData.word1 = FilterGroup::boule;
		shape->setSimulationFilterData(filterData);
		name = string("trigger"s + to_string(number));
		shape->setName(name.c_str());
		return false;
	}

	void BasicColider::place(float pos_x, float pos_y, float pos_z)
	{
		position = PxVec3(pos_x, pos_y, pos_z);
		body->setGlobalPose(PxTransform(PxVec3(pos_x, pos_y, pos_z)));
	}

	void BasicColider::trigger()
	{
		if (!triggered) {
			triggered = true;
			pred();
		}
	}
}
