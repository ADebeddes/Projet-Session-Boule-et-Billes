#include "stdafx.h"
#include "Bonus.h"
#include "MoteurWindows.h"
namespace PM3D {

	int Bonus::count = 1;

	Bonus::Bonus(CDispositifD3D11* pDispositif_, string path, int terrain_, LPCWSTR fong_file, bool need_filter) :
		StaticObject(pDispositif_, path, fong_file, need_filter)
	{
		terrain = terrain_;
		uid = count;
		count++;
		InitPhysique();
	}

	void Bonus::InitPhysique()
	{
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		PxShape* shape = rMoteur.Moteur_Physique.gPhysics->createShape(PxSphereGeometry(0.4f), *rMoteur.Moteur_Physique.gMaterial, true);
		const PxTransform localTm = PxTransform(PxVec3(0, 0, 0));
		
		PxFilterData filterData;
		filterData.word0 = FilterGroup::colider;
		filterData.word1 = FilterGroup::boule;
		shape->setSimulationFilterData(filterData);
		name = string("bonus"s + to_string(uid));
		shape->setName(name.c_str());

		body = rMoteur.Moteur_Physique.gPhysics->createRigidStatic(PxTransform(PxVec3(0, 0, 0)));

		body->attachShape(*shape);
	}
	void Bonus::Anime(float tempsEcoule)
	{
		const auto tm = body->getGlobalPose();
		pos = XMFLOAT3(tm.p.x, tm.p.y, tm.p.z);
	}
	void Bonus::place(float x, float y, float z)
	{
		body->setGlobalPose(PxTransform(PxVec3(x, y, z)));
		auto tm = body->getGlobalPose();
		matWorld = XMMatrixTranslation(tm.p.x, tm.p.y, tm.p.z);
	}
}