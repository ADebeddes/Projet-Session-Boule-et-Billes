#pragma once
#include "PxPhysicsAPI.h"
#include "DynamicObject.h"

using namespace physx;
using namespace std;
namespace PM3D
{
class Sphere : public DynamicObject
{
public:
	
	Sphere(CDispositifD3D11* pDispositif_, string path, char* name = "default");
	void InitPhysique(char* nom) override;
	void UpdatePhysique(float scale) override;
	void Anime(float tempsEcoule) override;
};
}



