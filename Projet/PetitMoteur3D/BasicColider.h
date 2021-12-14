#pragma once 
#include "PxPhysicsAPI.h"
#include <functional>
#include <string>

using namespace physx;
using namespace std;
namespace PM3D
{

	class BasicColider
	{
	public:

		static int count;


		BasicColider(int x, int y, int z);

		bool initPhysique(float x, float y, float z);

		void place(float pos_x, float pos_y, float pos_z);

		void trigger();
		
		std::function<void(void)> pred;

		int number;

		string name;

		PxRigidStatic* body;

		PxVec3 position;

		bool triggered{ false };
		
		
		
	};
}