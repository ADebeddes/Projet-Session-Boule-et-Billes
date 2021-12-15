#pragma once
#include "Bonus.h"

namespace PM3D {
	class AgrandirBalle : public Bonus {

	public:
		AgrandirBalle(CDispositifD3D11* pDispositif_, string path, int terrain, LPCWSTR fong_file = L"SimplePhong.fx", bool need_filter = false);
		void ActiverEffet(DynamicObject& obj) override;
	};

	class DiminuerBalle : public Bonus {

	public: 
		DiminuerBalle(CDispositifD3D11* pDispositif_, string path, int terrain, LPCWSTR fong_file = L"SimplePhong.fx", bool need_filter = false);
		void ActiverEffet(DynamicObject& obj) override;
	};

	class DiminuerAutreBalle : public Bonus {

	public :
		DiminuerAutreBalle(CDispositifD3D11* pDispositif_, string path, int terrain, LPCWSTR fong_file = L"SimplePhong.fx", bool need_filter = false);
		void ActiverEffet(DynamicObject& obj) override;
	};


	//class BalleFantome : public Bonus {

	//public : 
	//	BalleFantome(CDispositifD3D11* pDispositif_, string path, LPCWSTR fong_file = L"SimplePhong.fx", bool need_filter = false);
	//	void ActiverEffet(DynamicObject& obj) override;

	//};
}

