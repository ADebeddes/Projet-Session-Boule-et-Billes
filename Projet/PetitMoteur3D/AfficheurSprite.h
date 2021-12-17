#pragma once
#include "d3dx11effect.h"
#include "Objet3D.h"



namespace PM3D
{

class CDispositifD3D11;

class CSommetSprite
{
public:
	CSommetSprite() = default;
	CSommetSprite(const XMFLOAT3& position, const XMFLOAT2& coordTex)
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

class CAfficheurSprite : public CObjet3D
{
public:
	CAfficheurSprite(CDispositifD3D11* _pDispositif);
	virtual ~CAfficheurSprite();
	virtual void Draw() override;

	void AjouterSprite(const std::string& NomTexture, int _x, int _y, int _dx = 0, int _dy = 0, std::string function = "");
	void AjouterMenu(const std::string& NomTexture, int _x, int _y, int _dx = 0, int _dy = 0);
	void AjouterSpriteTexte(ID3D11ShaderResourceView* pTexture, std::string _name, int _x, int _y);
	void unDisplay(std::string name);
	void Display(std::string name);
	bool onScreen;

private:
	class CSprite
	{
	public:
		ID3D11ShaderResourceView* pTextureD3D;

		XMMATRIX matPosDim;
		std::string name;
		float pX = -1.0f;
		float pY = -1.0f;
		float dX = -1.0f;
		float dY = -1.0f;
		bool display;
		CSprite()
			: display(true)
			, pTextureD3D(nullptr)
		{
		}
	};
	
	static CSommetSprite sommets[6];
	ID3D11Buffer* pVertexBuffer;
	CDispositifD3D11* pDispositif;

	ID3D11Buffer* pConstantBuffer;
	ID3DX11Effect* pEffet;
	ID3DX11EffectTechnique* pTechnique;
	ID3DX11EffectPass* pPasse;
	ID3D11InputLayout* pVertexLayout;

	ID3D11SamplerState* pSampleState;

	// Tous nos sprites
	std::vector<std::unique_ptr<CSprite>> tabSprites;

	void InitEffet();
};

} // namespace PM3D
