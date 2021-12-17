#pragma once
#include "d3dx11effect.h"
#include "Objet3D.h"
#include "AfficheurPanneau.h"

namespace PM3D
{

    class CDispositifD3D11;

    class ParticleManager
    {
    public:

        ParticleManager(CDispositifD3D11* _pDispositif, XMFLOAT3 _pos, int _duration);
        ~ParticleManager();

        CAfficheurPanneau* pAfficheurParticule;

        XMFLOAT3& where_to_play;

        bool playing;

        int max_duration;

        int current_duration;

        unsigned int particle_number = 0;

        std::vector< XMFLOAT3> directions{};

        void AjouterParticle(const std::string& NomTexture = "snow.dds", float _dx = 0.1f, float _dy = 0.1);

        void  play();

        void UpdatePosition(XMFLOAT3 new_pos);

    };
}

