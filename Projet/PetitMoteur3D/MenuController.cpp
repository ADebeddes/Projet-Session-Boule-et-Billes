#include "stdafx.h"
#include "MenuController.h"
#include "MoteurWindows.h"

using namespace std;
namespace PM3D
{
	MenuController::MenuController(CDispositifD3D11* _pDispositif) : pDispositif(_pDispositif) {

		InitShader();
		auto e_largeur = pDispositif->GetLargeur();
		auto e_hauteur = pDispositif->GetHauteur();
		
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();

		AfficheurMenuPrincipal = new CAfficheurSprite(pDispositif);
		const Gdiplus::FontFamily oFamily(L"Arial", nullptr);
		pPolice1 = make_unique<Gdiplus::Font>(&oFamily, 48.0f, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
		TitrePrincipal = new CAfficheurTexte(_pDispositif, 350, 48, pPolice1.get());
		string sTitre = "Boule et Billes";
		wstring w_sTitre(sTitre.begin(), sTitre.end());
		TitrePrincipal->Ecrire(w_sTitre);
		AfficheurMenuPrincipal->AjouterSpriteTexte(TitrePrincipal->GetTextureView(),"TitrePrincipal", e_largeur / 2, e_hauteur / 8 * 2);

		AfficheurMenuPrincipal->AjouterSprite("Play.dds", e_largeur / 2, (e_hauteur / 10) * 5, e_largeur / 4, e_hauteur / 10);
		AfficheurMenuPrincipal->AjouterSprite("Settings.dds", e_largeur / 2, (e_hauteur / 10) * 7, e_largeur / 4, e_hauteur / 10);
		AfficheurMenuPrincipal->AjouterSprite("Quit.dds", e_largeur / 2, (e_hauteur / 10) * 9, e_largeur / 4, e_hauteur / 10);
		//AfficheurMenuPrincipal->AjouterMenu("menu.dds", 0, e_Hauteur, e_largeur, e_Hauteur);
	
		AfficheurMenuPrincipal->onScreen = true;

		AfficheurOption = new CAfficheurSprite(pDispositif);
		TitreOption = new CAfficheurTexte(_pDispositif, 200, 48, pPolice1.get());
		string sOption = "Settings";
		wstring w_sOption(sOption.begin(), sOption.end());
		TitreOption->Ecrire(w_sOption);
		AfficheurOption->AjouterSpriteTexte(TitreOption->GetTextureView(), "TitreOption", e_largeur / 2, e_hauteur / 8 * 2);

		pPolice2 = make_unique<Gdiplus::Font>(&oFamily, 24.0f, Gdiplus::FontStyleBold, Gdiplus::UnitPixel);
		optionEnnemies = new CAfficheurTexte(_pDispositif, 450, 24, pPolice2.get());
		string sEnemies = "Count of ennemies : " + to_string(rMoteur.pEntityManager->enemies.size());
		wstring w_sEnemies(sEnemies.begin(), sEnemies.end());
		optionEnnemies->Ecrire(w_sEnemies);

		optionPleinEcran = new CAfficheurTexte(_pDispositif, 450, 24, pPolice2.get());
		string sPleinEcran = "Fullscreen :";
		wstring w_sPleinEcran(sPleinEcran.begin(), sPleinEcran.end());
		optionPleinEcran->Ecrire(w_sPleinEcran);

		optionResolution = new CAfficheurTexte(_pDispositif, 450, 24, pPolice2.get());
		string sResolution = "Current Resolution : " + to_string(e_largeur) + "x" + to_string(e_hauteur);
		wstring w_sResolution(sResolution.begin(), sResolution.end());
		optionResolution->Ecrire(w_sResolution);

		optionShader = new CAfficheurTexte(_pDispositif, 450, 24, pPolice2.get());
		string sShader = "Current Shader : " + shaders.at(rMoteur.pPanneauPE->TECHNIQUE_USED);
		wstring w_sShader(sShader.begin(), sShader.end());
		optionShader->Ecrire(w_sShader);

		AfficheurOption->AjouterSpriteTexte(optionShader->GetTextureView(), "optionShader", (e_largeur / 6) * 2, ((e_hauteur / 10) * 4) - (e_hauteur / 30));
		AfficheurOption->AjouterSpriteTexte(optionResolution->GetTextureView(), "optionResolution", (e_largeur / 6) * 2, ((e_hauteur / 10) * 5) - (e_hauteur / 30));
		AfficheurOption->AjouterSpriteTexte(optionPleinEcran->GetTextureView(), "optionPleinEcran",(e_largeur / 6) * 2, ((e_hauteur / 10) * 6) - (e_hauteur / 30));
		AfficheurOption->AjouterSpriteTexte(optionEnnemies->GetTextureView(), "optionEnnemies",(e_largeur / 6) * 2, ((e_hauteur / 10) * 7) - (e_hauteur / 30));
		AfficheurOption->AjouterSprite("CheckBoxFenetre.dds", (e_largeur / 6) * 5, (e_hauteur / 10) * 6, e_largeur / 15, e_hauteur / 15);
		AfficheurOption->AjouterSprite("CheckBoxPleinEcran.dds", (e_largeur / 6) * 5, (e_hauteur / 10) * 6, e_largeur / 15, e_hauteur / 15);
		AfficheurOption->unDisplay("CheckBoxPleinEcran.dds");
		AfficheurOption->AjouterSprite("Plus.dds", (e_largeur / 6) * 4, (e_hauteur / 10) * 7, e_largeur / 15, e_hauteur / 15);
		AfficheurOption->AjouterSprite("Moins.dds", (e_largeur / 6) * 5, (e_hauteur / 10) * 7, e_largeur / 15, e_hauteur / 15);
		AfficheurOption->AjouterSprite("Return.dds", e_largeur / 2 , (e_hauteur / 10) * 9, e_largeur / 4, e_hauteur / 10);
		AfficheurOption->AjouterSprite("Left.dds", (e_largeur / 6) * 4, (e_hauteur / 10) * 5, e_largeur / 15, e_hauteur / 15, "Resolution");
		AfficheurOption->AjouterSprite("Right.dds", (e_largeur / 6) * 5, (e_hauteur / 10) * 5, e_largeur / 15, e_hauteur / 15, "Resolution");
		AfficheurOption->AjouterSprite("Left.dds", (e_largeur / 6) * 4, (e_hauteur / 10) * 4, e_largeur / 15, e_hauteur / 15, "Shader");
		AfficheurOption->AjouterSprite("Right.dds", (e_largeur / 6) * 5, (e_hauteur / 10) * 4, e_largeur / 15, e_hauteur / 15, "Shader");
		//AfficheurOption->AjouterMenu("menu.dds", 0, e_hauteur, e_largeur, e_hauteur);

		AfficheurOption->onScreen = false;


	}
	void MenuController::setOn()
	{
		onMenu = true;
		AfficheurMenuPrincipal->onScreen = true;
	}
	void MenuController::setOff()
	{
		onMenu = false;
		AfficheurMenuPrincipal->onScreen = false;
		AfficheurOption->unDisplay("Plus.dds");
		AfficheurOption->unDisplay("Moins.dds");
		AfficheurOption->unDisplay("optionEnnemies");
	}
	void MenuController::InitShader()
	{
		shaders.push_back("Default");
		shaders.push_back("Radial Blur");
		shaders.push_back("Sketch");
		shaders.push_back("Black And White");
	}
	bool MenuController::doAction(string s)
	{
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		if (s == "Play.dds") {
			rMoteur.sceneManager.addEntities(rMoteur.pEntityManager);
			return false;
		}
		if (s == "Settings.dds") {
			AfficheurMenuPrincipal->onScreen = false;
			AfficheurOption->onScreen = true;
		}
		if (s == "CheckBoxFenetre.dds") {
			rMoteur.pDispositif->GetSwapChain()->SetFullscreenState(TRUE, nullptr);
			AfficheurOption->Display("CheckBoxPleinEcran.dds");
			AfficheurOption->unDisplay("CheckBoxFenetre.dds");
		}
		if (s == "CheckBoxPleinEcran.dds") {
			rMoteur.pDispositif->GetSwapChain()->SetFullscreenState(FALSE, nullptr);
			AfficheurOption->Display("CheckBoxFenetre.dds");
			AfficheurOption->unDisplay("CheckBoxPleinEcran.dds");
		}
		if (s == "Plus.dds") {
			rMoteur.pEntityManager->AddEntity(rMoteur.PremierePosition, rMoteur.pDispositif);
			string sEnemies = "Count of ennemies : " + to_string(rMoteur.pEntityManager->enemies.size());
			wstring w_sEnemies(sEnemies.begin(), sEnemies.end());
			optionEnnemies->Ecrire(w_sEnemies);
		}
		if (s == "Moins.dds") {
			rMoteur.pEntityManager->DelEntity();
			string sEnemies = "Count of ennemies : " + to_string(rMoteur.pEntityManager->enemies.size());
			wstring w_sEnemies(sEnemies.begin(), sEnemies.end());
			optionEnnemies->Ecrire(w_sEnemies);
		}
		if (s == "Left.ddsResolution") {
			rMoteur.pDispositif->setLargeur(1024);
			rMoteur.pDispositif->setHauteur(768);
			rMoteur.pDispositif->Resize();
			string sResolution = "Current Resolution : " + to_string(rMoteur.pDispositif->GetLargeur()) + "x" + to_string(rMoteur.pDispositif->GetHauteur());
			wstring w_sResolution(sResolution.begin(), sResolution.end());
			optionResolution->Ecrire(w_sResolution);
		}
		if (s == "Right.ddsResolution") {
			rMoteur.pDispositif->setLargeur(1920);
			rMoteur.pDispositif->setHauteur(1080);
			rMoteur.pDispositif->Resize();
			string sResolution = "Current Resolution : " + to_string(rMoteur.pDispositif->GetLargeur()) + "x" + to_string(rMoteur.pDispositif->GetHauteur());
			wstring w_sResolution(sResolution.begin(), sResolution.end());
			optionResolution->Ecrire(w_sResolution);
		}
		if (s == "Left.ddsShader") {
			if (rMoteur.pPanneauPE->TECHNIQUE_USED != 0) {
				rMoteur.pPanneauPE->TECHNIQUE_USED = (rMoteur.pPanneauPE->TECHNIQUE_USED - 1) % 4;
			}
			else {
				rMoteur.pPanneauPE->TECHNIQUE_USED = 3;
			}
			string sShader = "Current Shader : " + shaders.at(rMoteur.pPanneauPE->TECHNIQUE_USED);
			wstring w_sShader(sShader.begin(), sShader.end());
			optionShader->Ecrire(w_sShader);
		}
		if (s == "Right.ddsShader") {
			rMoteur.pPanneauPE->TECHNIQUE_USED = (rMoteur.pPanneauPE->TECHNIQUE_USED + 1) % 4;
			string sShader = "Current Shader : " + shaders.at(rMoteur.pPanneauPE->TECHNIQUE_USED);
			wstring w_sShader(sShader.begin(), sShader.end());
			optionShader->Ecrire(w_sShader);
		}
		if (s == "Return.dds") {
			AfficheurMenuPrincipal->onScreen = true;
			AfficheurOption->onScreen = false;
		}
		//pSwapChain->SetFullscreenState(FALSE, nullptr);
		if (s == "Quit.dds") {
			PostQuitMessage(0);
		}
		return true;
	}
}