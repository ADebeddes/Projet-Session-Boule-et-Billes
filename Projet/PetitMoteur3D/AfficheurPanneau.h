#pragma once
#include "d3dx11effect.h"
#include "Objet3D.h"

namespace PM3D
{

	class CDispositifD3D11;

	class CSommetPanneau
	{
	public:
		CSommetPanneau() = default;
		CSommetPanneau(const XMFLOAT3& position, const XMFLOAT2& coordTex)
			: m_Position(position)
			, m_CoordTex(coordTex)
		{
		}

	public:
		static UINT numElements;
		static D3D11_INPUT_ELEMENT_DESC layout[];

		XMFLOAT3 m_Position;
		XMFLOAT2 m_CoordTex;
	};

	class CAfficheurPanneau : public CObjet3D
	{
	public:
		CAfficheurPanneau(CDispositifD3D11* _pDispositif);
		virtual ~CAfficheurPanneau();
		virtual void Draw() override;

		void AjouterPanneau(const std::string& NomTexture, const XMFLOAT3& _position,
			float _dx = 0, float _dy = 0, bool _idle = false);
		bool onScreen;
		
		class CPanneau
		{
		public:
			ID3D11ShaderResourceView* pTextureD3D;

			bool idle;
			XMMATRIX matPosDim;
			std::string name;
			XMFLOAT3 position;
			XMFLOAT2 dimension;
			CPanneau() : pTextureD3D(nullptr)
			{

			}
		};

	private:
		

		XMMATRIX matWorld;
		static CSommetPanneau sommets[6];
		ID3D11Buffer* pVertexBuffer;
		CDispositifD3D11* pDispositif;

		ID3D11Buffer* pConstantBuffer;
		ID3DX11Effect* pEffet;
		ID3DX11EffectTechnique* pTechnique;
		ID3DX11EffectPass* pPasse;
		ID3D11InputLayout* pVertexLayout;

		ID3D11SamplerState* pSampleState;


		void InitEffet();
	public:
		// Tous nos sprites
		std::vector<std::unique_ptr<CPanneau>> tabPanneau;
	};

} // namespace PM3D
