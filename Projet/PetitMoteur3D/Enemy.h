﻿#pragma once
#include "DynamicObject.h"
#include "AfficheurPanneau.h"
#include "ParticleManager.h"

//
//   TEMPLATE : Enemy
//
//   BUT : Classe des autres boules de neige 
//
//
namespace PM3D {
	class Enemy
	{
	public:
		DynamicObject& enemyCharacter;
		ParticleManager* pParticleManager;
		const char* nom;

		PxVec3 lastCheckPointPos;
	
		Enemy(PxVec3 pos, DynamicObject& object,const char* nom);
		void Anime();

		void Crash();

		void Growth();

		void Shrink();

		void Respawn();
	};
}

