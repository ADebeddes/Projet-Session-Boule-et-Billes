#pragma once
#include "AfficheurSprite.h"
#include "AfficheurTexte.h"
#include <string>
#include "SceneManager.h"
using namespace std;
namespace PM3D
{
	class MenuController {
	public:
		MenuController(CDispositifD3D11* _pDispositif);
		CAfficheurSprite* AfficheurMenuPrincipal;
		CAfficheurSprite* AfficheurOption;
		CDispositifD3D11* pDispositif;
		CAfficheurTexte* TitrePrincipal;
		CAfficheurTexte* TitreOption;
		CAfficheurTexte* optionEnnemies;
		CAfficheurTexte* optionPleinEcran;
		CAfficheurTexte* optionResolution;
		CAfficheurTexte* optionShader;

		vector<string> shaders;
		unique_ptr<Gdiplus::Font> pPolice1;
		unique_ptr<Gdiplus::Font> pPolice2;
		bool onMenu = true;
		void setOn();
		void setOff();
		void InitShader();
		bool doAction(string s);
	};
}