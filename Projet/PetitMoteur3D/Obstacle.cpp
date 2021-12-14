#include "stdafx.h"
#include "Obstacle.h"
#include "MoteurWindows.h"



namespace PM3D
{
	

	Obstacle::Obstacle(CDispositifD3D11* pDispositif_, string path, LPCWSTR fong_file , bool need_filter )
		: StaticObject(pDispositif_, path, fong_file , need_filter)

	{

		InitPhysique(path);
		
	
	}

	void Obstacle::InitPhysique(string path)
	{
		if(!InitMesh(path) )
		{ 
			createMesh(path);
		}
		
		loadMesh(path);
		
		
	}

	bool Obstacle::InitMesh(string path)
	{
		if (possible_shape.find(path) != possible_shape.end()) 
		{
			return true ; 
		}
		return false ; 
	}
	void Obstacle::loadMesh(string path)
	{
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();


		physx::PxShape* shape = possible_shape[path]; 

		body = rMoteur.Moteur_Physique.gPhysics->createRigidStatic(PxTransform(PxVec3(0, 0, 0)));

		body->attachShape(*shape);
	}

	void Obstacle::Anime(float tempsEcoule)
	{
		
	}


	void Obstacle::createMesh(string path)
	{
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		PxTolerancesScale scale;
		

		PxTriangleMeshDesc meshDesc;
		meshDesc.points.count = (int)oi.object.points_.size();
		meshDesc.points.stride = sizeof(PxVec3);
		meshDesc.points.data = oi.object.points_.data();

		meshDesc.triangles.count = (int)oi.object.indices_.size() / 3;
		meshDesc.triangles.stride = 3 * sizeof(PxU32);
		meshDesc.triangles.data = oi.object.indices_.data();

		PxTriangleMesh* aTriangleMesh = nullptr;

		try {
			aTriangleMesh = rMoteur.Moteur_Physique.gCooking->createTriangleMesh(meshDesc,
				rMoteur.Moteur_Physique.gPhysics->getPhysicsInsertionCallback());
		}
		catch (...) {

		}
		PxTriangleMeshGeometry geometry = physx::PxTriangleMeshGeometry(aTriangleMesh);

		physx::PxShape* shape = rMoteur.Moteur_Physique.gPhysics->createShape(geometry, *rMoteur.Moteur_Physique.gMaterial, true);

		PxFilterData filterData;
		filterData.word0 = FilterGroup::obstacle;
		filterData.word1 = FilterGroup::boule ;
		shape->setSimulationFilterData(filterData);
		shape->setName("Obstacle");

		possible_shape.emplace(path, shape); 


	}

	void Obstacle::place(float x, float y, float z)
	{
		
		body->setGlobalPose(PxTransform(PxVec3(x,y, z)));
		auto tm = body->getGlobalPose(); 
		matWorld =  XMMatrixTranslation(tm.p.x, tm.p.y, tm.p.z);
		pos = XMFLOAT3(tm.p.x, tm.p.y, tm.p.z);
	}
}