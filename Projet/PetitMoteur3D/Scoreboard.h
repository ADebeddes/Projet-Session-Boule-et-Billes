#pragma once
#include "AfficheurSprite.h"
#include "AfficheurTexte.h"
#include "AfficheurPanneau.h"
using namespace std;
namespace PM3D {
	class Scoreboard
	{
	public:
		std::unique_ptr<Gdiplus::Font> pPolice;
		CAfficheurSprite* pAfficheurScoreboard;
		vector< CAfficheurTexte*> textes;
		bool init = false;

		Scoreboard();
		void Init();
		void Anime();

	};
}

