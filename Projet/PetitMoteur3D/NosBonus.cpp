#include "stdafx.h"
#include "NosBonus.h"
#include "resource.h"
#include "MoteurWindows.h"

namespace PM3D {
	AgrandirBalle::AgrandirBalle(CDispositifD3D11* pDispositif_, string path, LPCWSTR fong_file, bool need_filter) :
		Bonus(pDispositif_, path, fong_file, need_filter)
	{
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		SetTexture(rMoteur.GetTextureManager().GetNewTexture(L"up.dds", pDispositif));
	}
	void AgrandirBalle::ActiverEffet(DynamicObject& obj)
	{
		if (!triggered) {
			obj.scale = obj.scale * 1.2f;
			triggered = true;
			visible = false;
		}
	}
	DiminuerBalle::DiminuerBalle(CDispositifD3D11* pDispositif_, string path, LPCWSTR fong_file, bool need_filter)
		: Bonus(pDispositif_, path, fong_file, need_filter)
	{
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		SetTexture(rMoteur.GetTextureManager().GetNewTexture(L"down.dds", pDispositif));
	}
	void DiminuerBalle::ActiverEffet(DynamicObject& obj)
	{
		if (!triggered) {
			obj.scale = obj.scale * 0.8f;
			triggered = true;
			visible = false;
		}
	}
	DiminuerAutreBalle::DiminuerAutreBalle(CDispositifD3D11* pDispositif_, string path, LPCWSTR fong_file, bool need_filter)
		: Bonus(pDispositif_, path, fong_file, need_filter)
	{
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		SetTexture(rMoteur.GetTextureManager().GetNewTexture(L"eclair.dds", pDispositif));
	}
	void DiminuerAutreBalle::ActiverEffet(DynamicObject& obj)
	{

		if (!triggered) {

			triggered = true;
			visible = false;


			CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();

			auto nom = obj.object_name;


			if (strcmp(rMoteur.pEntityManager->pPlayer->playerCharacter.object_name , nom) != 0) {
				rMoteur.pEntityManager->pPlayer->playerCharacter.scale = rMoteur.pEntityManager->pPlayer->playerCharacter.scale* 0.8f;
			}

			for (auto enemyEntity : rMoteur.pEntityManager->enemies){

				if (strcmp(nom, enemyEntity->enemyCharacter.object_name) != 0) {
					enemyEntity->enemyCharacter.scale = enemyEntity->enemyCharacter.scale *0.8f;
				}
			}
		}
	}
	
	
}
