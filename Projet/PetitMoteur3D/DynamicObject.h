#pragma once
#include "PxPhysicsAPI.h"
#include "ImportedObject.h"

using namespace physx;
using namespace std;
namespace PM3D
{
	//Nos objets dynamics (Gloablement tous nos objets qui auront un PxRigidDynamic
	class DynamicObject : public ImportedObject
	{
	public : 
		DynamicObject(CDispositifD3D11* pDispositif_, string path);

		
		
		PxRigidDynamic* body;
		float scale;
		char* object_name = { "default" };
	};
}

