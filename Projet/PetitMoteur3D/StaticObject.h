#pragma once
#include "PxPhysicsAPI.h"
#include "ImportedObject.h"

using namespace physx;
using namespace std;
namespace PM3D
{
	//Nos objets statiques (PxRigidStatic)
	class StaticObject : public ImportedObject
	{
	public:
		StaticObject(CDispositifD3D11* pDispositif_, string path, LPCWSTR fong_file, bool need_filter, bool shadow_ok = false);
		PxRigidStatic* body;
	};
}


