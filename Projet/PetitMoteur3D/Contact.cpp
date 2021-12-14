#include "stdafx.h"
#include "Contact.h"
#include <iostream>
#include "MoteurWindows.h"


void Contact::onContactModify(PxContactModifyPair* const pairs, PxU32 count)
{
	CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
	for (int i = 0; i < count * 1.0; i++)
	{
		auto nom0 = pairs[i].shape[0]->getName();
		auto nom1 = pairs[i].shape[1]->getName();
		if (strcmp(nom0, "Player") == 0)
		{

			for (auto collider : rMoteur.sceneManager.collider_pool)
			{
				if (strcmp(collider->name.c_str(), nom1) == 0)
				{
					rMoteur.pEntityManager->pPlayer->lastCheckPointPos = collider->position;
					collider->trigger();
				}
			}

			for (auto bonus : rMoteur.sceneManager.bonus_pool) {
				if (strcmp(bonus->name.c_str(), nom1) == 0)
				{
					bonus->ActiverEffet(rMoteur.pEntityManager->pPlayer->playerCharacter);
				}
			}
		}

		for (auto j = 0; j < rMoteur.pEntityManager->enemies.size(); j++) {

			for (auto collider : rMoteur.sceneManager.collider_pool) {
				if (strcmp(nom0, rMoteur.pEntityManager->enemies[j]->nom) == 0 && strcmp(collider->name.c_str(), nom1) == 0)
				{


					rMoteur.pEntityManager->enemies[j]->lastCheckPointPos = collider->position;
				}
				if (strcmp(nom1, rMoteur.pEntityManager->enemies[j]->nom) == 0 && strcmp(collider->name.c_str(), nom0) == 0)
				{

					rMoteur.pEntityManager->enemies[j]->lastCheckPointPos = collider->position;

				}
			}

			for (auto bonus : rMoteur.sceneManager.bonus_pool) {
				if (strcmp(nom0, rMoteur.pEntityManager->enemies[j]->nom) == 0 && strcmp(bonus->name.c_str(), nom1) == 0)
				{
					bonus->ActiverEffet(rMoteur.pEntityManager->enemies[j]->enemyCharacter);
				}
				if (strcmp(nom1, rMoteur.pEntityManager->enemies[j]->nom) == 0 && strcmp(bonus->name.c_str(), nom0) == 0)
				{

					bonus->ActiverEffet(rMoteur.pEntityManager->enemies[j]->enemyCharacter);

				}
			}

		}

	}
}

void Contact::onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
{
	PX_UNUSED(pairHeader);
	CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
	for (int k = 0; k < (int)nbPairs; k++)
	{
		auto currentPair = pairs[k];


		auto nom0 = currentPair.shapes[0]->getName();
		auto nom1 = currentPair.shapes[1]->getName();

		if (nom0 == nullptr) {

			if (strcmp(nom1, "Player") == 0) {
				if (rMoteur.pEntityManager->pPlayer->playerCharacter.body->getLinearVelocity().magnitude() < 2)
				{
					rMoteur.pEntityManager->pPlayer->Shrink();
				}
				else
				{
					rMoteur.pEntityManager->pPlayer->Growth();
				}
			}
			for (auto j = 0; j < rMoteur.pEntityManager->enemies.size(); j++) {

				if (strcmp(nom1, rMoteur.pEntityManager->enemies[j]->nom) == 0)
				{
					if (rMoteur.pEntityManager->enemies[j]->enemyCharacter.body->getLinearVelocity().magnitude() < 2)
					{
						rMoteur.pEntityManager->enemies[j]->Shrink();
					}
					else
					{
						rMoteur.pEntityManager->enemies[j]->Growth();
					}
				}

			}

		}
		else if (nom1 == nullptr) {

			const bool test = strcmp(nom0, "Player") == 0;
			if (strcmp(nom0, "Player") == 0) {

				if (rMoteur.pEntityManager->pPlayer->playerCharacter.body->getLinearVelocity().magnitude() < 2)
				{
					rMoteur.pEntityManager->pPlayer->Shrink();
				}
				else
				{
					rMoteur.pEntityManager->pPlayer->Growth();
				}
			}

			for (auto j = 0; j < rMoteur.pEntityManager->enemies.size(); j++) {

				if (strcmp(nom0, rMoteur.pEntityManager->enemies[j]->nom) == 0)
					if (rMoteur.pEntityManager->enemies[j]->enemyCharacter.body->getLinearVelocity().magnitude() < 2)
					{
						rMoteur.pEntityManager->enemies[j]->Shrink();
					}
					else
					{
						rMoteur.pEntityManager->enemies[j]->Growth();
					}

			}

		}
		else {



			if (strcmp(nom0, "Player") == 0 && strcmp(nom1, "Obstacle") == 0)
			{

				rMoteur.pEntityManager->pPlayer->Crash();
			}
			if (strcmp(nom1, "Player") == 0 && strcmp(nom0, "Obstacle") == 0)
			{
				rMoteur.pEntityManager->pPlayer->Crash();
			}

			for (auto i = 0; i < rMoteur.pEntityManager->enemies.size(); i++) {



				if (strcmp(nom0, "Player") == 0 && strcmp(nom1, rMoteur.pEntityManager->enemies[i]->nom) == 0)
				{
					rMoteur.pEntityManager->pPlayer->Crash();
					rMoteur.pEntityManager->enemies[i]->Crash();
				}
				if (strcmp(nom1, "Player") == 0 && strcmp(nom0, rMoteur.pEntityManager->enemies[i]->nom) == 0)
				{

					rMoteur.pEntityManager->pPlayer->Crash();
					rMoteur.pEntityManager->enemies[i]->Crash();
				}

				if (strcmp(nom0, rMoteur.pEntityManager->enemies[i]->nom) == 0 && strcmp(nom1, "Obstacle") == 0)
				{
					rMoteur.pEntityManager->enemies[i]->Crash();
				}
				if (strcmp(nom1, rMoteur.pEntityManager->enemies[i]->nom) == 0 && strcmp(nom0, "Obstacle") == 0)
				{

					rMoteur.pEntityManager->enemies[i]->Crash();
				}


				for (auto j = 0; j < rMoteur.pEntityManager->enemies.size(); j++) {

					if (strcmp(nom0, rMoteur.pEntityManager->enemies[j]->nom) == 0 && strcmp(nom1, rMoteur.pEntityManager->enemies[i]->nom) == 0)
					{


						rMoteur.pEntityManager->enemies[i]->Crash();
						rMoteur.pEntityManager->enemies[j]->Crash();
					}
					if (strcmp(nom1, rMoteur.pEntityManager->enemies[j]->nom) == 0 && strcmp(nom0, rMoteur.pEntityManager->enemies[i]->nom) == 0)
					{

						rMoteur.pEntityManager->enemies[i]->Crash();
						rMoteur.pEntityManager->enemies[j]->Crash();
					}


				}
			}
		}
	}
}

void Contact::onTrigger(PxTriggerPair* pairs, PxU32 count)
{
	PX_UNUSED(pairs);
	PX_UNUSED(count);

	cout << "bonk";

}



