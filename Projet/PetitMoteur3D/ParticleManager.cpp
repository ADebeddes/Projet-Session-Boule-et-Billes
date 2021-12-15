#include "stdafx.h"
#include "ParticleManager.h"
#include "resource.h"
#include "MoteurWindows.h"
#include "util.h"
#include "DispositifD3D11.h"

namespace PM3D {
	ParticleManager::ParticleManager(CDispositifD3D11* _pDispositif, XMFLOAT3 _pos, int _duration)
			: where_to_play(_pos)
			, duration(_duration)
	{
		pAfficheurParticule = new CAfficheurPanneau(_pDispositif);
		pAfficheurParticule->onScreen = true;
		pAfficheurParticule->tabPanneau.clear();
	}
	void ParticleManager::AjouterParticle(const std::string& NomTexture, float _dx, float _dy)
	{

		pAfficheurParticule->AjouterPanneau(NomTexture, where_to_play, _dx, _dy);
		directions.push_back(XMFLOAT3((rand() / float(RAND_MAX)), (rand() / float(RAND_MAX)), (rand() / float(RAND_MAX))));

	}
	void ParticleManager::play()
	{
		for (int i = 0; i < directions.size(); i++)
		{
			pAfficheurParticule->tabPanneau[i].get()->position = XMFLOAT3(
				pAfficheurParticule->tabPanneau[i].get()->position.x + directions[i].x,
				pAfficheurParticule->tabPanneau[i].get()->position.y + directions[i].y,
				pAfficheurParticule->tabPanneau[i].get()->position.z + directions[i].z
			);
		}
	}

	void ParticleManager::UpdatePosition(XMFLOAT3 new_pos) {
		for (auto& p : pAfficheurParticule->tabPanneau) {
			p->position = new_pos;
		}
	}
}