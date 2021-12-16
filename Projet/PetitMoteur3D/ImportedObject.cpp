#pragma once
#include "stdafx.h"
#include "ImportedObject.h"
#include "sommetbloc.h"
#include "util.h"
#include "DispositifD3D11.h"

#include "resource.h"
#include "MoteurWindows.h"

#include "Texture.h"

#include <array>

#include <iostream>
#include <type_traits>
#include "CDIManipulateur.h"

namespace PM3D
{

	

	struct ShadersParams
	{
		XMMATRIX matWorldViewProj;	// la matrice totale 
		XMMATRIX matWorld;			// matrice de transformation dans le monde 
		XMMATRIX vLumiere; 			// la position de la source d'éclairage (Point)
		XMVECTOR vCamera; 			// la position de la caméra
		XMVECTOR vAEcl; 			// la valeur ambiante de l'éclairage
		XMVECTOR vAMat; 			// la valeur ambiante du matériau
		XMVECTOR vDEcl; 			// la valeur diffuse de l'éclairage 
		XMVECTOR vDMat; 			// la valeur diffuse du matériau 
	};

	struct ShadowShadersParams // toujours un multiple de 16 pour les constantes
	{
		XMMATRIX matWorldViewProj;	// la matrice totale 
		XMMATRIX matWorldViewProjLight;	// WVP pour lumiere 
		XMMATRIX matWorld;			// matrice de transformation dans le monde 
		XMVECTOR vLumiere; 			// la position de la source d'éclairage (Point)
		XMVECTOR vCamera; 			// la position de la caméra
		XMVECTOR vAEcl; 			// la valeur ambiante de l'éclairage
		XMVECTOR vAMat; 			// la valeur ambiante du matériau
		XMVECTOR vDEcl; 			// la valeur diffuse de l'éclairage 
		XMVECTOR vDMat; 			// la valeur diffuse du matériau 
		XMVECTOR vSEcl; 			// la valeur spéculaire de l'éclairage 
		XMVECTOR vSMat; 			// la valeur spéculaire du matériau 
		float puissance;
		int bTex;					// Texture ou materiau 
		XMFLOAT2 remplissage;
	};


	//  FONCTION : CBlocEffet1, constructeur paramètré 
	//  BUT :	Constructeur d'une classe de bloc avec effet voir 6.5
	//  PARAMÈTRES:		
	//		dx, dy, dz:	dimension en x, y, et z
	//		pDispositif: pointeur sur notre objet dispositif
	ImportedObject::ImportedObject(CDispositifD3D11* pDispositif_, string path, LPCWSTR fong_file, bool need_filter, bool shadow_ok )
		: pDispositif(pDispositif_) // Prendre en note le dispositif
		, matWorld(XMMatrixIdentity())
		, fong_file(fong_file)
		, need_filter(need_filter)
		, pVertexBuffer(nullptr)
		, pIndexBuffer(nullptr)
		, pConstantBuffer(nullptr)
		, pEffet(nullptr)
		, pTechnique(nullptr)
		, pVertexLayout(nullptr)
		, pSampleState(0)
	{



		/********************* import****************************/
		oi = ObjectImporter{};
		oi.import(path);

		
		 size_x = abs(oi.object.max_x - oi.object.min_x);
		 size_y = abs(oi.object.max_y - oi.object.min_y);
		 size_z = abs(oi.object.max_z - oi.object.min_z);

		points =  oi.object.points_;
		sommets = &oi.object.sommets_[0];
		sommets_size = (UINT)oi.object.sommets_.size();
		index = &oi.object.index_[0];
		index_size = (UINT)oi.object.index_.size();
		has_texture = oi.object.has_textture_;

		/********************* import****************************/

		ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(CSommetBloc) * sommets_size;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = &oi.object.sommets_[0];
		pVertexBuffer = nullptr;

		DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer), DXE_CREATIONVERTEXBUFFER);
		
		// Création de l'index buffer et copie des indices
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_IMMUTABLE;
		bd.ByteWidth = sizeof(unsigned int) * index_size;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;

		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = &oi.object.index_[0];
		pIndexBuffer = nullptr;

		DXEssayer(pD3DDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer),
			DXE_CREATIONINDEXBUFFER);

		// Initialisation de l'effet
		if (shadow_ok)
		{
			InitEffetShadows(); 
		}
		else
		{
			InitEffet();
		}

		auto material = CMaterial();
		material.NomMateriau = "";
		material.transparent = false;
		material.Ambient.x = material.Ambient.y = material.Ambient.z = 0.6f;
		material.Diffuse.x = material.Diffuse.y = material.Diffuse.z = 0.6f;
		material.Specular.x = material.Specular.y = material.Specular.z = 0.9f;




		Material.emplace_back(material);
		
	}

	void ImportedObject::Draw()
	{
		// Obtenir le contexte
		ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext();

		// Choisir la topologie des primitives
		pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		// Source des sommets
		const UINT stride = sizeof(CSommetBloc);
		const UINT offset = 0;
		pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

		// Source des index
		pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

		// input layout des sommets
		pImmediateContext->IASetInputLayout(pVertexLayout);

		// Initialiser et sélectionner les «constantes» de l'effet
		ShadersParams sp;
		XMMATRIX viewProj = CMoteurWindows::GetInstance().GetMatViewProj();

		sp.matWorldViewProj = XMMatrixTranspose(matWorld * viewProj);
		sp.matWorld = XMMatrixTranspose(matWorld);

		sp.vLumiere = lights_DL_PtL;
		sp.vCamera = XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f);
		sp.vAEcl = XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f);
		sp.vAMat = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		sp.vDEcl = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		sp.vDMat = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
		pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &sp, 0, 0);

		ID3DX11EffectConstantBuffer* pCB = pEffet->GetConstantBufferByName("param");  // Nous n'avons qu'un seul CBuffer
		pCB->SetConstantBuffer(pConstantBuffer);

		// Activation de la texture 
		
		// Le sampler state 
		ID3DX11EffectSamplerVariable* variableSampler;
		variableSampler = pEffet->GetVariableByName("SampleState")->AsSampler();
		variableSampler->SetSampler(0, pSampleState);
		
		
		ID3DX11EffectShaderResourceVariable* variableTexture;
		//variableTexture = pEffet->GetVariableByName("textureEntrees")->AsShaderResource();
		variableTexture = pEffet->GetVariableByName("textureEntrees")->AsShaderResource();
		variableTexture->SetResourceArray(resourcesView.data(), 0, (uint32_t)resourcesView.size());

		if (!has_texture)
		{
			ID3DX11EffectShaderResourceVariable* variableFiltre;
			variableFiltre = pEffet->GetVariableByName("filter")->AsShaderResource();
			variableFiltre->SetResource(map_filter);
		}
				
		// **** Rendu de l'objet
		pPasse->Apply(0, pImmediateContext);

		pCB->SetConstantBuffer(pConstantBuffer);
		pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &sp, 0, 0);
		pImmediateContext->DrawIndexed((UINT)index_size, 0, 0);
	
	}

	void ImportedObject::DrawShadows()
	{
		
		// ***** OMBRES ---- Valide pour les deux rendus
		// Obtenir le contexte
		ID3D11DeviceContext* pImmediateContext = pDispositif->GetImmediateContext();

	// Choisir la topologie des primitives
	pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Index buffer
	pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Vertex buffer
	UINT stride = sizeof(CSommetBloc);
	const UINT offset = 0;
	pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

	// ***** OMBRES ---- Premier Rendu - Création du Shadow Map
	// Utiliser la surface de la texture comme surface de rendu
	pImmediateContext->OMSetRenderTargets(1, &pRenderTargetView,
		pDepthStencilView);

	// Effacer le shadow map 
	float Couleur[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	pImmediateContext->ClearRenderTargetView(pRenderTargetView, Couleur);
	pImmediateContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Modifier les dimension du viewport
	pDispositif->SetViewPortDimension(512, 512);

	// Choix de la technique
	pTechnique = pEffet->GetTechniqueByName("ShadowMap");
	pPasse = pTechnique->GetPassByIndex(0);

	// input layout des sommets
	pImmediateContext->IASetInputLayout(pVertexLayoutShadow);

	// Initialiser et sélectionner les «constantes» de l'effet
	ShadowShadersParams sp;
	sp.matWorldViewProjLight = XMMatrixTranspose(matWorld * mVPLight);

	// Nous n'avons qu'un seul CBuffer
	ID3DX11EffectConstantBuffer* pCB = pEffet->GetConstantBufferByName("param");
	pCB->SetConstantBuffer(pConstantBuffer);
	pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &sp, 0, 0);

	// **** Rendu de l'objet
	pPasse->Apply(0, pImmediateContext);

	pCB->SetConstantBuffer(pConstantBuffer);
	pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &sp, 0, 0);
	pImmediateContext->DrawIndexed((UINT)index_size, 0, 0);

	// ***** OMBRES ---- Deuxième Rendu - Affichage de l'objet avec ombres
		// Ramener la surface de rendu
	ID3D11RenderTargetView* tabRTV[1];
	tabRTV[0] = pDispositif->GetRenderTargetView();
	pImmediateContext->OMSetRenderTargets(1,
		tabRTV,
		pDispositif->GetDepthStencilView());

	// Dimension du viewport - défaut
	pDispositif->ResetViewPortDimension();

	// Choix de la technique
	pTechnique = pEffet->GetTechniqueByName("MiniPhong");
	pPasse = pTechnique->GetPassByIndex(0);

	// Initialiser et sélectionner les «constantes» de l'effet
	XMMATRIX viewProj = CMoteurWindows::GetInstance().GetMatViewProj();

	sp.matWorldViewProj = XMMatrixTranspose(matWorld * viewProj);
	sp.matWorld = XMMatrixTranspose(matWorld);

	sp.vLumiere = XMVectorSet(400.0f, 150.0f, -500.0f, 1.0f);
	sp.vCamera = XMVectorSet(0.0f, 30.0f, -5.0f, 1.0f);
	sp.vAEcl = XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f);
	sp.vDEcl = XMVectorSet(1.0f, 1.0f, 1.0f, 1.0f);
	sp.vSEcl = XMVectorSet(0.6f, 0.6f, 0.6f, 1.0f);
	// Le sampler state
	ID3DX11EffectSamplerVariable* variableSampler;
	variableSampler = pEffet->GetVariableByName("SampleState")->AsSampler();
	variableSampler->SetSampler(0, pSampleState);

	// input layout des sommets
	pImmediateContext->IASetInputLayout(pVertexLayout);

	ID3DX11EffectShaderResourceVariable* pShadowMap;
	pShadowMap = pEffet->GetVariableByName("ShadowTexture")->AsShaderResource();
	pShadowMap->SetResource(pShadowMapView);

	pDispositif->SetNormalRSState();

	// Dessiner les subsets non-transparents
	
	sp.vAMat = XMLoadFloat4(&Material[0].Ambient);
	sp.vDMat = XMLoadFloat4(&Material[0].Diffuse);
	sp.vSMat = XMLoadFloat4(&Material[0].Specular);
	sp.puissance = Material[0].Puissance;

	// Activation de la texture ou non
	
		ID3DX11EffectShaderResourceVariable* variableTexture;
		variableTexture =
			pEffet->GetVariableByName("textureEntree")->AsShaderResource();
		variableTexture->SetResource(resourcesView[0]);
		sp.bTex = 1;
	

	// **** Rendu de l'objet
	pPasse->Apply(0, pImmediateContext);

	pCB->SetConstantBuffer(pConstantBuffer);
	pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &sp, 0, 0);
	pImmediateContext->DrawIndexed((UINT)index_size, 0, 0);
		
	//pDispositif->ResetViewPortDimension(); 


	}

	
	
	void ImportedObject::SetTexture(CTexture* pTexture)
	{
		if (pTexture != nullptr)
		resourcesView.push_back(pTexture->GetD3DTexture());
	}

	void ImportedObject::SetFilter(CTexture* pTexture)
	{
		if (pTexture != nullptr)
			map_filter = pTexture->GetD3DTexture();
	}


	

	float ImportedObject::getHeightAt(float x, float z)
	{
		auto it = std::find_if(points.begin(), points.end(), [&](XMFLOAT3 p) { return  abs(p.x - x) < 1.0  && abs( p.z - z ) <1.0 ; });
		if (it == points.end())
		{
			return 0.0f; 
		}
		return it->y; 
	}

	ImportedObject::~ImportedObject()
	{
		Material.clear();

		DXRelacher(pConstantBuffer);
		DXRelacher(pSampleState);

		DXRelacher(pEffet);
		DXRelacher(pVertexLayout);
		DXRelacher(pIndexBuffer);
		DXRelacher(pVertexBuffer);

		DXRelacher(pShadowMapView);
		DXRelacher(pRenderTargetView);
		DXRelacher(pTextureShadowMap);
		DXRelacher(pDepthStencilView);
		DXRelacher(pDepthTexture);

		DXRelacher(pVertexLayoutShadow);

		

	
	}

	

	void ImportedObject::InitEffet()
	{
		InitDepthBuffer();
		InitMatricesShadowMap();
		// Compilation et chargement du vertex shader
		ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

		// Création d'un tampon pour les constantes du VS
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(ShadersParams);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		pD3DDevice->CreateBuffer(&bd, nullptr, &pConstantBuffer);

		// Pour l'effet
		ID3DBlob* pFXBlob = nullptr;

		DXEssayer(D3DCompileFromFile(fong_file, 0, 0, 0,
			"fx_5_0", 0, 0,
			&pFXBlob, nullptr),
			DXE_ERREURCREATION_FX);


		D3DX11CreateEffectFromMemory(pFXBlob->GetBufferPointer(), pFXBlob->GetBufferSize(), 0, pD3DDevice, &pEffet);

		pFXBlob->Release();

		pTechnique = pEffet->GetTechniqueByIndex(0);
		pPasse = pTechnique->GetPassByIndex(0);

		// Créer l'organisation des sommets pour le VS de notre effet
		D3DX11_PASS_SHADER_DESC effectVSDesc;
		pPasse->GetVertexShaderDesc(&effectVSDesc);

		D3DX11_EFFECT_SHADER_DESC effectVSDesc2;
		effectVSDesc.pShaderVariable->GetShaderDesc(effectVSDesc.ShaderIndex, &effectVSDesc2);

		const void* vsCodePtr = effectVSDesc2.pBytecode;
		const unsigned vsCodeLen = effectVSDesc2.BytecodeLength;

		pVertexLayout = nullptr;
		DXEssayer(pD3DDevice->CreateInputLayout(CSommetBloc::layout,
			CSommetBloc::numElements,
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

		// Création de l’état de sampling 
		pD3DDevice->CreateSamplerState(&samplerDesc, &pSampleState);
	}


	void ImportedObject::InitEffetShadows()
	{
		InitDepthBuffer();
		InitMatricesShadowMap();

		// Compilation et chargement du vertex shader
		ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

		// Création d'un tampon pour les constantes du VS
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(ShadersParams);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		HRESULT hr = pD3DDevice->CreateBuffer(&bd, nullptr, &pConstantBuffer);

		// Pour l'effet
		ID3DBlob* pFXBlob = nullptr;

		DXEssayer(D3DCompileFromFile(L"MiniPhongSM.fx", 0, 0, 0,
			"fx_5_0", D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, 0, &pFXBlob, 0),
			DXE_ERREURCREATION_FX);

		D3DX11CreateEffectFromMemory(pFXBlob->GetBufferPointer(), pFXBlob->GetBufferSize(), 0, pD3DDevice, &pEffet);

		pFXBlob->Release();

		pTechnique = pEffet->GetTechniqueByIndex(0);
		pPasse = pTechnique->GetPassByIndex(0);

		// Créer l'organisation des sommets pour les VS de notre effet
		D3DX11_PASS_SHADER_DESC effectVSDesc;
		D3DX11_EFFECT_SHADER_DESC effectVSDesc2;
		const void* vsCodePtr;
		unsigned vsCodeLen;

		// 1 pour le shadowmap
		pTechnique = pEffet->GetTechniqueByName("ShadowMap");
		pPasse = pTechnique->GetPassByIndex(0);
		pPasse->GetVertexShaderDesc(&effectVSDesc);
		effectVSDesc.pShaderVariable->GetShaderDesc(effectVSDesc.ShaderIndex,
			&effectVSDesc2);

		vsCodePtr = effectVSDesc2.pBytecode;
		vsCodeLen = effectVSDesc2.BytecodeLength;

		pVertexLayout = nullptr;

		DXEssayer(pD3DDevice->CreateInputLayout(CSommetBloc::layout,
			CSommetBloc::numElements,
			vsCodePtr,
			vsCodeLen,
			&pVertexLayoutShadow),
			DXE_CREATIONLAYOUT);

		// 2 pour miniphong
		pTechnique = pEffet->GetTechniqueByName("MiniPhong");
		pPasse = pTechnique->GetPassByIndex(0);
		pPasse->GetVertexShaderDesc(&effectVSDesc);
		effectVSDesc.pShaderVariable->GetShaderDesc(effectVSDesc.ShaderIndex,
			&effectVSDesc2);

		vsCodePtr = effectVSDesc2.pBytecode;
		vsCodeLen = effectVSDesc2.BytecodeLength;

		pVertexLayout = nullptr;

		DXEssayer(pD3DDevice->CreateInputLayout(CSommetBloc::layout,
			CSommetBloc::numElements,
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

		D3D11_TEXTURE2D_DESC textureDesc;
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

		// Description de la texture
		ZeroMemory(&textureDesc, sizeof(textureDesc));

		// Cette texture sera utilisée comme cible de rendu et 
		// comme ressource de shader
		textureDesc.Width = SHADOWMAP_DIM;
		textureDesc.Height = SHADOWMAP_DIM;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		// Création de la texture
		pD3DDevice->CreateTexture2D(&textureDesc, nullptr, &pTextureShadowMap);

		// VUE - Cible de rendu
		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;

		// Création de la vue.
		pD3DDevice->CreateRenderTargetView(pTextureShadowMap,
			&renderTargetViewDesc,
			&pRenderTargetView);

		// VUE – Ressource de shader
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		// Création de la vue.
		pD3DDevice->CreateShaderResourceView(pTextureShadowMap,
			&shaderResourceViewDesc,
			&pShadowMapView);

		
	}
	
	
	
	void ImportedObject::InitMatricesShadowMap()
	{
		// Matrice de la vision vu par la lumière - Le point TO est encore 0,0,0
		mVLight = XMMatrixLookAtLH(
			XMVectorSet(5.0f, 50.0f, -5.0f, 1.0f),
			XMVectorSet(0.0f, 10.0f, 0.0f, 1.0f),
			XMVectorSet(0.2f, 0.2f, 0.2f, 1.0f));

		const float champDeVision = XM_PI / 8;  // 45 degrés
		const float ratioDAspect = 1.0f; 	// 512/512
		const float planRapproche = 0.10f; 	// Pas besoin d'être trop près
		const float planEloigne = 1000.0f;	// Suffisemment pour avoir tous les objets
		mPLight = XMMatrixPerspectiveFovLH(champDeVision,
			ratioDAspect,
			planRapproche,
			planEloigne);

		mVPLight = mVLight * mPLight;
	}

	void ImportedObject::InitDepthBuffer()
	{
		ID3D11Device* pD3DDevice = pDispositif->GetD3DDevice();

		D3D11_TEXTURE2D_DESC depthTextureDesc;
		ZeroMemory(&depthTextureDesc, sizeof(depthTextureDesc));
		depthTextureDesc.Width = 512;
		depthTextureDesc.Height = 512;
		depthTextureDesc.MipLevels = 1;
		depthTextureDesc.ArraySize = 1;
		depthTextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthTextureDesc.SampleDesc.Count = 1;
		depthTextureDesc.SampleDesc.Quality = 0;
		depthTextureDesc.Usage = D3D11_USAGE_DEFAULT;
		depthTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthTextureDesc.CPUAccessFlags = 0;
		depthTextureDesc.MiscFlags = 0;

		DXEssayer(
			pD3DDevice->CreateTexture2D(&depthTextureDesc, nullptr, &pDepthTexture),
			DXE_ERREURCREATIONTEXTURE);

		// Création de la vue du tampon de profondeur (ou de stencil)
		D3D11_DEPTH_STENCIL_VIEW_DESC descDSView;
		ZeroMemory(&descDSView, sizeof(descDSView));
		descDSView.Format = depthTextureDesc.Format;
		descDSView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSView.Texture2D.MipSlice = 0;
		DXEssayer(
			pD3DDevice->CreateDepthStencilView(pDepthTexture, &descDSView, &pDepthStencilView),
			DXE_ERREURCREATIONDEPTHSTENCILTARGET);
	}



} // namespace PM3D

