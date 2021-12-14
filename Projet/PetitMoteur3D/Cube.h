#pragma once
#include "PxPhysicsAPI.h"
#include "DynamicObject.h"

using namespace physx;
using namespace std;
namespace PM3D
{
	class Cube : public DynamicObject
	{
	public:

		Cube(CDispositifD3D11* pDispositif_, string path, char* name = "default");
		void InitPhysique(char* name) override;
		void UpdatePhysique(float scale) override;
		void Anime(float tempsEcoule);
	};
}

