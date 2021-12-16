#pragma once
#include "PxPhysicsAPI.h"
#include "StaticObject.h"

using namespace physx;
using namespace std;
namespace PM3D
{
	class Terrain : public StaticObject
	{
	public:
		Terrain(CDispositifD3D11* pDispositif_, string path, PxVec3 pos = PxVec3(0, 0, 0), LPCWSTR fong_file = L"Terrain.fx", bool need_filter = true );
		void InitPhysique(PxVec3 pos);

		//Creer le Mesh si le .dat n'existe pas 
		void createMesh(PxVec3 pos );

		//Load le .dat
		bool loadMesh();
		PxCollection* collection{};
		std::string fileName{};
	};
}