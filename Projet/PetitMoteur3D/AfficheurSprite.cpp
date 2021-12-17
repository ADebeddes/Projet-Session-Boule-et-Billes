#include "StdAfx.h"
#include "AfficheurSprite.h"
#include "resource.h"
#include "MoteurWindows.h"
#include "util.h"
#include "DispositifD3D11.h"


namespace PM3D
{
struct ShadersParams
{
	XMMATRIX matWVP;	// la matrice totale 
};

// Definir l'organisation de notre sommet
D3D11_INPUT_ELEMENT_DESC CSommetSprite::layout[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0},
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0}
};

UINT CSommetSprite::numElements = ARRAYSIZE(layout);

CSommetSprite CAfficheurSprite::sommets[6] =
{
				CSommetSprite(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f)),
				CSommetSprite(XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f)),
				CSommetSprite(XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f)),
				CSommetSprite(XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f)),
				CSommetSprite(XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f)),
				CSommetSprite(XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f))
};

CAfficheurSprite::CAfficheurSprite(CDispositifD3D11* _pDispositif)
	: pDispositif(_pDispositif)
	, pVertexBuffer(nullptr)
	, pConstantBuffer(nullptr)
	, pEffet(nullptr)
	, pTechnique(nullptr)
	, pPasse(nullptr)
	, pVertexLayout(nullptr)
	, pSampleState(nullptr)
	, onScreen(false)
{
	// Création du vertex buffer et copie des sommets
	ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(sommets);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA InitData;
	ZeroMemory(&InitData, sizeof(InitData));
	InitData.pSysMem = sommets;

	DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer),
		DXE_CREATIONVERTEXBUFFER);

	// Initialisation de l'effet
	InitEffet();
}

CAfficheurSprite ::~CAfficheurSprite()
{
	DXRelacher(pPasse);
	DXRelacher(pTechnique);
	DXRelacher(pConstantBuffer);
	DXRelacher(pSampleState);

	DXRelacher(pEffet);
	DXRelacher(pVertexLayout);
	DXRelacher(pVertexBuffer);

	tabSprites.clear();
}

void CAfficheurSprite::InitEffet()
{
	// Compilation et chargement du vertex shader
	ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

	// Création d'un tampon pour les constantes de l'effet
	D3D11_BUFFER_DESC bd;
	ZeroMemory(&bd, sizeof(bd));

	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof(ShadersParams);
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	pD3DDevice->CreateBuffer(&bd, nullptr, &pConstantBuffer);

	// Pour l'effet
	ID3DBlob* pFXBlob = nullptr;

	DXEssayer(D3DCompileFromFile(L"Sprite1.fx", 0, 0, 0,
		"fx_5_0", 0, 0,
		&pFXBlob, 0),
		DXE_ERREURCREATION_FX);

	D3DX11CreateEffectFromMemory(pFXBlob->GetBufferPointer(),
		pFXBlob->GetBufferSize(), 0, pD3DDevice, &pEffet);

	pFXBlob->Release();

	pTechnique = pEffet->GetTechniqueByIndex(0);
	pPasse = pTechnique->GetPassByIndex(0);

	// Créer l'organisation des sommets pour le VS de notre effet
	D3DX11_PASS_SHADER_DESC effectVSDesc;
	pPasse->GetVertexShaderDesc(&effectVSDesc);

	D3DX11_EFFECT_SHADER_DESC effectVSDesc2;
	effectVSDesc.pShaderVariable->GetShaderDesc(effectVSDesc.ShaderIndex,
		&effectVSDesc2);

	const void *vsCodePtr = effectVSDesc2.pBytecode;
	const unsigned vsCodeLen = effectVSDesc2.BytecodeLength;

	DXEssayer(pD3DDevice->CreateInputLayout(
		CSommetSprite::layout,
		CSommetSprite::numElements,
		vsCodePtr,
		vsCodeLen,
		&pVertexLayout),
		DXE_CREATIONLAYOUT);

	// Initialisation des paramètres de sampling de la texture
	D3D11_SAMPLER_DESC samplerDesc;

	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 4;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Création de l'état de sampling
	pD3DDevice->CreateSamplerState(&samplerDesc, &pSampleState);
}

void CAfficheurSprite::Draw()
{
	if (onScreen) {
		// Obtenir le contexte
		ID3D11DeviceContext* pImmediateContext =
			pDispositif->GetImmediateContext();

		// Choisir la topologie des primitives
		pImmediateContext->IASetPrimitiveTopology(
			D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Source des sommets
		const UINT stride = sizeof(CSommetSprite);
		const UINT offset = 0;
		pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride,
			&offset);

		// input layout des sommets
		pImmediateContext->IASetInputLayout(pVertexLayout);

		// Le sampler state
		ID3DX11EffectSamplerVariable* variableSampler;
		variableSampler = pEffet->GetVariableByName("SampleState")->AsSampler();
		variableSampler->SetSampler(0, pSampleState);

		ID3DX11EffectConstantBuffer* pCB =
			pEffet->GetConstantBufferByName("param");
		ID3DX11EffectShaderResourceVariable* variableTexture;
		variableTexture =
			pEffet->GetVariableByName("textureEntree")->AsShaderResource();

		pDispositif->ActiverMelangeAlpha();

		CDIManipulateur& GestionnaireDeSaisie =
			CMoteurWindows::GetInstance().GetGestionnaireDeSaisie();

		bool hover = false;
		// Faire le rendu de tous nos sprites
		for (auto& sprite : tabSprites)
		{
			if (sprite->display){ 
				// Initialiser et sélectionner les «constantes» de l'effet
				ShadersParams sp;
				sp.matWVP = XMMatrixTranspose(sprite->matPosDim);
				pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr,
					&sp, 0, 0);

				pCB->SetConstantBuffer(pConstantBuffer);

				auto e_largeur = pDispositif->GetLargeur();
				auto e_hauteur = pDispositif->GetHauteur();
				float debX = sp.matWVP.r[0].vector4_f32[0];
				float debY = sp.matWVP.r[1].vector4_f32[1];

				if (GestionnaireDeSaisie.x >= (int)((e_largeur / sprite->pX) - (e_largeur / sprite->dX) * 0.5) &&
					GestionnaireDeSaisie.y >= (int)((e_hauteur / sprite->pY) - (e_hauteur / sprite->dY)) &&
					GestionnaireDeSaisie.x <= (int)((e_largeur / sprite->pX) + (e_largeur / sprite->dX) * 0.5) &&
					GestionnaireDeSaisie.y <= (int)((e_hauteur / sprite->pY))) {
					hover = true;
					CMoteurWindows::GetInstance().HoveredOption = sprite->name;
				}
				if (!hover)CMoteurWindows::GetInstance().HoveredOption = "None";
				// Activation de la texture
				variableTexture->SetResource(sprite->pTextureD3D);

				pPasse->Apply(0, pImmediateContext);

				// **** Rendu de l'objet
				pDispositif->ActiverMelangeAlpha();
				pImmediateContext->Draw(6, 0);
				pDispositif->DesactiverMelangeAlpha();
			}
		}

		pDispositif->DesactiverMelangeAlpha();
	}
}

void CAfficheurSprite::AjouterSprite(const std::string& NomTexture,
	int _x, int _y,
	int _dx, int _dy
	,std::string function)
{
	float x, y, dx, dy;
	float posX, posY;
	float facteurX, facteurY;

	// Initialisation de la texture
	CGestionnaireDeTextures& TexturesManager =
		CMoteurWindows::GetInstance().GetTextureManager();

	std::wstring ws(NomTexture.begin(), NomTexture.end());

	std::unique_ptr<CSprite> pSprite = std::make_unique<CSprite>();;
	pSprite->pTextureD3D =
		TexturesManager.GetNewTexture(ws.c_str(), pDispositif)->GetD3DTexture();

	ID3D11Texture2D* pTextureInterface = 0;
	ID3D11Resource* res;
	pSprite->pTextureD3D->GetResource(&res);
	res->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
	D3D11_TEXTURE2D_DESC descText;
	pTextureInterface->GetDesc(&descText);
	DXRelacher(res);
	DXRelacher(pTextureInterface);

	// Obtenir les dimensions de la texture si _dx et _dy sont à 0;
	if (_dx == 0 && _dy == 0)
	{
		ID3D11Resource* pResource;
		ID3D11Texture2D* pTextureInterface = 0;
		pSprite->pTextureD3D->GetResource(&pResource);
		pResource->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
		D3D11_TEXTURE2D_DESC desc;
		pTextureInterface->GetDesc(&desc);

		DXRelacher(pResource);
		DXRelacher(pTextureInterface);

		dx = float(desc.Width);
		dy = float(desc.Height);
	}
	else
	{
		dx = float(_dx);
		dy = float(_dy);
	}

	// Dimension en facteur
	facteurX = dx * 2.0f / pDispositif->GetLargeur();
	facteurY = dy * 2.0f / pDispositif->GetHauteur();

	// Position en coordonnées logiques
	// 0,0 pixel = -1,1   
	x = float(_x - dx * 0.5);
	y = float(_y);

	pSprite->name = NomTexture + function;

	pSprite->pX = pDispositif->GetLargeur() / (float)_x;
	pSprite->pY = pDispositif->GetHauteur() / (float)_y;
	pSprite->dX = pDispositif->GetLargeur() / dx;
	pSprite->dY = pDispositif->GetHauteur() / dy;

	posX = x * 2.0f / pDispositif->GetLargeur() - 1.0f;
	posY = 1.0f - y * 2.0f / pDispositif->GetHauteur();

	pSprite->matPosDim = XMMatrixScaling(facteurX, facteurY, 1.0f) *
		XMMatrixTranslation(posX, posY, 0.0f);

	// On l'ajoute à notre vecteur
	tabSprites.push_back(std::move(pSprite));
}


void CAfficheurSprite::AjouterMenu(const std::string& NomTexture,
	int _x, int _y,
	int _dx, int _dy)
{
	float x, y, dx, dy;
	float posX, posY;
	float facteurX, facteurY;

	// Initialisation de la texture
	CGestionnaireDeTextures& TexturesManager =
		CMoteurWindows::GetInstance().GetTextureManager();

	std::wstring ws(NomTexture.begin(), NomTexture.end());

	std::unique_ptr<CSprite> pSprite = std::make_unique<CSprite>();;
	pSprite->pTextureD3D =
		TexturesManager.GetNewTexture(ws.c_str(), pDispositif)->GetD3DTexture();

	// Obtenir les dimensions de la texture si _dx et _dy sont à 0;
	if (_dx == 0 && _dy == 0)
	{
		ID3D11Resource* pResource;
		ID3D11Texture2D* pTextureInterface = 0;
		pSprite->pTextureD3D->GetResource(&pResource);
		pResource->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
		D3D11_TEXTURE2D_DESC desc;
		pTextureInterface->GetDesc(&desc);

		DXRelacher(pResource);
		DXRelacher(pTextureInterface);

		dx = float(desc.Width);
		dy = float(desc.Height);
	}
	else
	{
		dx = float(_dx);
		dy = float(_dy);
	}

	// Dimension en facteur
	facteurX = dx * 2.0f / pDispositif->GetLargeur();
	facteurY = dy * 2.0f / pDispositif->GetHauteur();

	// Position en coordonnées logiques
	// 0,0 pixel = -1,1   
	x = float(_x);
	y = float(_y);

	posX = x * 2.0f / pDispositif->GetLargeur() - 1.0f;
	posY = 1.0f - y * 2.0f / pDispositif->GetHauteur();

	pSprite->matPosDim = XMMatrixScaling(facteurX, facteurY, 1.0f) *
		XMMatrixTranslation(posX, posY, 0.0f);

	// On l'ajoute à notre vecteur
	tabSprites.push_back(std::move(pSprite));
}


void CAfficheurSprite::AjouterSpriteTexte(
	ID3D11ShaderResourceView* pTexture, std::string _name, int _x, int _y)
{
	std::unique_ptr<CSprite> pSprite = std::make_unique<CSprite>();
	pSprite->pTextureD3D = pTexture;
	pSprite->name = _name;
	// Obtenir la dimension de la texture;
	ID3D11Resource* pResource;
	ID3D11Texture2D *pTextureInterface = 0;
	pSprite->pTextureD3D->GetResource(&pResource);
	pResource->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
	D3D11_TEXTURE2D_DESC desc;
	pTextureInterface->GetDesc(&desc);


	DXRelacher(pResource);
	DXRelacher(pTextureInterface);

	const float dx = float(desc.Width);
	const float dy = float(desc.Height);

	// Dimension en facteur
	const float facteurX = dx * 2.0f / pDispositif->GetLargeur();
	const float facteurY = dy * 2.0f / pDispositif->GetHauteur();

	// Position en coordonnées logiques
	// 0,0 pixel = -1,1   
	const float x = float(_x - dx * 0.5);
	const float y = float(_y);

	const float posX = x * 2.0f / pDispositif->GetLargeur() - 1.0f;
	const float posY = 1.0f - y * 2.0f / pDispositif->GetHauteur();

	pSprite->matPosDim = XMMatrixScaling(facteurX, facteurY, 1.0f) *
		XMMatrixTranslation(posX, posY, 0.0f);

	// On l'ajoute à notre vecteur
	tabSprites.push_back(std::move(pSprite));
}

void CAfficheurSprite::unDisplay(string name)
{
	for (auto& sprite : tabSprites)
	{
		if (sprite->name == name)sprite->display = false;
	}
}

void CAfficheurSprite::Display(string name)
{
	for (auto& sprite : tabSprites)
	{
		if (sprite->name == name)sprite->display = true;
	}
}

} // namespace PM3D
