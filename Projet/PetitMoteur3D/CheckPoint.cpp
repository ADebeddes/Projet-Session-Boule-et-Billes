#include "stdafx.h"
#include "CheckPoint.h"

CheckPoint::CheckPoint(CDispositifD3D11* pDispositif_, string path, LPCWSTR fong_file, bool need_filter) : StaticObject(pDispositif_, path, fong_file, need_filter)
{
	InitPhysique();
}

void CheckPoint::InitPhysique()
{
}
