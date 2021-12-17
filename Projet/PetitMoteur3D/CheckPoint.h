#pragma once
#include "StaticObject.h"

//
//   TEMPLATE : CheckPoint
//
//   BUT : Permet au Boule de conserver une position en cas d'echec
//
//   COMMENTAIRES :
//
//
class CheckPoint : public StaticObject
{

public:
	CheckPoint(CDispositifD3D11* pDispositif_, string path, LPCWSTR fong_file = L"SimplePhong.fx", bool need_filter = false);

	void InitPhysique();
};

