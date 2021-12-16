#pragma once
#include "StaticObject.h"
#include <map>

using namespace physx;
using namespace std;
namespace PM3D
{

	//
//   TEMPLATE : Obstacle
//
//   BUT : Classe des objets Static
//
    class Obstacle :
        public StaticObject
    {
    public:
        Obstacle(CDispositifD3D11* pDispositif_, string path ,int terrain, LPCWSTR fong_file = L"SimplePhong.fx", bool need_filter = false);
	
		void InitPhysique(string path);
		bool InitMesh(string path);
		void loadMesh(string path);
		void Anime(float tempsEcoule);
		void createMesh(string path);
		static std::map<string, PxShape*> possible_shape;

		int terrain;
		void place(float x ,float y, float z);
		
	};
}