#include "stdafx.h"
#include "StaticObject.h"
#include "resource.h"
#include "MoteurWindows.h"

using namespace physx;
using namespace std;
namespace PM3D
{
	StaticObject::StaticObject(CDispositifD3D11* pDispositif_, string path, LPCWSTR fong_file , bool need_filter)
		:ImportedObject(pDispositif_, path, fong_file , need_filter )
	{
	}
}

