
#include "stdafx.h"
#include "Terrain.h"
#include "resource.h"
#include "MoteurWindows.h"

namespace PM3D
{
	

	Terrain::Terrain(CDispositifD3D11* pDispositif_, string path, PxVec3 pos,LPCWSTR fong_file , bool need_filter )
		: StaticObject(pDispositif_, path,  fong_file,  need_filter )
	{
		fileName = path.substr(0, path.find("."));
		fileName = fileName + "serialized_.dat";
		
		
		InitPhysique(pos);

		
		matWorld = XMMatrixTranslation(pos.x, pos.y, pos.z);
	}

	void Terrain::InitPhysique(PxVec3 pos )
	{

		if (!loadMesh())
		{
			createMesh(pos);
		}

	}

	void Terrain::createMesh(PxVec3 pos )
	{
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		PxTolerancesScale scale;
		PxCookingParams params(scale);
		// disable mesh cleaning - perform mesh validation on development configurations
		//params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_CLEAN_MESH;
		// disable edge precompute, edges are set for each triangle, slows contact generation
		//params.meshPreprocessParams |= PxMeshPreprocessingFlag::eDISABLE_ACTIVE_EDGES_PRECOMPUTE;

		rMoteur.Moteur_Physique.gCooking->setParams(params);

		PxTriangleMeshDesc meshDesc;
		meshDesc.points.count = (int)oi.object.points_.size();
		meshDesc.points.stride = sizeof(PxVec3);
		meshDesc.points.data = oi.object.points_.data();

		meshDesc.triangles.count = (int)oi.object.indices_.size() / 3;
		meshDesc.triangles.stride = 3 * sizeof(PxU32);
		meshDesc.triangles.data = oi.object.indices_.data();

		bool truc = false;
		truc = rMoteur.Moteur_Physique.gCooking->validateTriangleMesh(meshDesc);
		PX_ASSERT(truc == true );

		auto aTriangleMesh = rMoteur.Moteur_Physique.gCooking->createTriangleMesh(meshDesc,
			rMoteur.Moteur_Physique.gPhysics->getPhysicsInsertionCallback());

		PxTriangleMeshGeometry geometry = physx::PxTriangleMeshGeometry(aTriangleMesh);
	

		physx::PxShape* shape = rMoteur.Moteur_Physique.gPhysics->createShape(geometry, *rMoteur.Moteur_Physique.gMaterial, true);

		PxFilterData filterData;
		filterData.word0 = FilterGroup::terrain;
		filterData.word1 = FilterGroup::boule || FilterGroup::obstacle;
		shape->setName("Terrain");
		shape->setSimulationFilterData(filterData);

		body = rMoteur.Moteur_Physique.gPhysics->createRigidStatic(PxTransform(pos));

		body->attachShape(*shape);

		PxSerializationRegistry* registry = PxSerialization::createSerializationRegistry(PxGetPhysics());

		// Create a collection and all objects for serialization
		collection = PxCreateCollection();
		collection->add(*body);
		PxSerialization::complete(*collection, *registry);

		// Serialize either to binary or RepX


		PxDefaultFileOutputStream outStream(fileName.c_str());

		// Binary
		PxSerialization::serializeCollectionToBinary(outStream, *collection, *registry);
		//~Binary

	}

	bool Terrain::loadMesh()
	{

		PxSerializationRegistry* registry = PxSerialization::createSerializationRegistry(PxGetPhysics());

		// Binary
			// Open file and get file size
		FILE* fp = fopen(fileName.c_str(), "rb");
		if (fp)
		{
			fseek(fp, 0, SEEK_END);
			unsigned fileSize = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			// Allocate aligned memory, load data and deserialize
			void* memory = malloc(fileSize + PX_SERIAL_FILE_ALIGN);
			void* memory128 = (void*)((size_t(memory) + PX_SERIAL_FILE_ALIGN) & ~(PX_SERIAL_FILE_ALIGN - 1));
			fread(memory128, 1, fileSize, fp);
			fclose(fp);
			collection = PxSerialization::createCollectionFromBinary(memory128, *registry);
			//~Binary
			return true;
		}
		return false;

	}



}
