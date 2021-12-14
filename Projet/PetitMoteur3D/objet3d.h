#pragma once
#include "dispositif.h"

using namespace DirectX;

namespace PM3D
{
	//  Classe : CObjet3D
	//
	//  BUT : 	Classe de base de tous nos objets 3D
	//
	class CObjet3D
	{
	public:

		XMFLOAT3 pos = XMFLOAT3(-100, -100, -100);
		bool visible = true;
		bool allwaysVisible = false;

		virtual ~CObjet3D() = default;

		virtual void Anime(float) {};
		virtual void Draw() = 0;
	};

} // namespace PM3D
