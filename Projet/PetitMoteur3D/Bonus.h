#pragma once
#include "StaticObject.h"
#include "DynamicObject.h"
namespace PM3D {
	class Bonus : public StaticObject
	{

	public:
		int uid;
		int terrain;
		static int count;
		string name;
		bool triggered = false;
		Bonus(CDispositifD3D11* pDispositif_, string path, int terrain,LPCWSTR fong_file = L"SimplePhong.fx", bool need_filter = false);

		void InitPhysique();
		void Anime(float tempsEcoule) override;
		void place(float x, float y, float z);
		virtual void ActiverEffet(DynamicObject& obj) = 0;
	};
}

