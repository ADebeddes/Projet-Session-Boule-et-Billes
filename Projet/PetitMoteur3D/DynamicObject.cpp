#include "stdafx.h"
#include "DynamicObject.h"

namespace PM3D
{
	DynamicObject::DynamicObject(CDispositifD3D11* pDispositif_, string path)
		: ImportedObject(pDispositif_, path), scale{1.0f}
	{
	}
}
