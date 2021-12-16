#include "stdafx.h"
#include "Scoreboard.h"
#include "resource.h"
#include "MoteurWindows.h"
#include <memory>
#include <iterator>
#include <string>
PM3D::Scoreboard::Scoreboard()
{
	
}

void PM3D::Scoreboard::Init()
{

	CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
	pAfficheurScoreboard = new CAfficheurSprite(rMoteur.pDispositif);
	int nb = static_cast<int>(rMoteur.pEntityManager->enemies.size()) + 1;
	int count = 1;
	const Gdiplus::FontFamily oFamily(L"Arial", nullptr);
	pPolice = std::make_unique<Gdiplus::Font>(&oFamily, 20.0f, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
	auto e_largeur = rMoteur.pDispositif->GetLargeur();
	auto e_Hauteur = rMoteur.pDispositif->GetHauteur();
	textes.clear();

	textes.push_back(new CAfficheurTexte(rMoteur.pDispositif, 135, 26, pPolice.get()));
	pAfficheurScoreboard->AjouterSpriteTexte(textes[textes.size() - 1]->GetTextureView(), "noms", e_largeur - 80, e_Hauteur / 20 * 2 + count*26);
	
	string nom = to_string(count) + "- "s + string(rMoteur.pEntityManager->pPlayer->nom);
	std::wstring w_nom(nom.begin(), nom.end());
	textes[textes.size() - 1]->Ecrire(w_nom);
	count++;

	for (auto enemy : rMoteur.pEntityManager->enemies) {

		textes.push_back(new CAfficheurTexte(rMoteur.pDispositif, 135, 26, pPolice.get()));
		pAfficheurScoreboard->AjouterSpriteTexte(textes[textes.size() - 1]->GetTextureView(), "noms", e_largeur - 80, e_Hauteur / 20 * 2 + count * 26);
		count++;
		string nom = to_string(count) + "- "s + string(enemy->nom);
		std::wstring w_nom(nom.begin(), nom.end());
		textes[textes.size() - 1]->Ecrire(w_nom);
	}
	pAfficheurScoreboard->onScreen = true;
	init = true;

}

void PM3D::Scoreboard::Anime()
{
	CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
	auto vec = rMoteur.sceneManager.PlusProcheFin();

	for (int i = 0; i < vec.size(); i++) {

		string s = to_string(i + 1) + "- "s + vec[i].first;
		if (i < 9) {
			s = " "s + s;
		}
		std::wstring w_nom(s.begin(), s.end());
		textes[i]->Ecrire(w_nom);
	}
}
