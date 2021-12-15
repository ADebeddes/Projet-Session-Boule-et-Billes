#pragma once
#include "DynamicObject.h"
#include "Sphere.h"
#include "Cube.h"
#include "AfficheurPanneau.h"
#include "ParticleManager.h"


namespace PM3D {
	class Player
	{
	public:
		DynamicObject& playerCharacter;
		float vitesse;
		char* nom;
		bool need_to_respawn = false; 
		PxVec3 lastCheckPointPos;
		PxVec3 lastCheckPointDir;
		ParticleManager* pParticleManager;
	
		//Player(PxVec3 pos, PxVec3 initialLookAt, DynamicObject& object);

		Player(PxVec3 pos, PxVec3 dir, Sphere& object);
		Player(PxVec3 pos, PxVec3 dir, Cube& object);

		//Recupere les inputs du joueur et deplace la boule, fait grossir la boule en fonction de la scale du joueur
		void Anime();

		//Diminue la scale du joueur en cas de crash
		void Crash();

		//Augmente la scale du joueur lorsqu'il est sur le terrain
		void Growth();

		void Shrink();

		//Fais respawn au dernier checkpoint 
		void Respawn();
		
	};
}

