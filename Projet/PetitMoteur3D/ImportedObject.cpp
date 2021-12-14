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

	//  FONCTION : CBlocEffet1, constructeur paramètré 
	//  BUT :	Constructeur d'une classe de bloc avec effet voir 6.5
	//  PARAMÈTRES:		
	//		dx, dy, dz:	dimension en x, y, et z
	//		pDispositif: pointeur sur notre objet dispositif
	ImportedObject::ImportedObject(CDispositifD3D11* pDispositif_, string path, LPCWSTR fong_file, bool need_filter )
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
		InitEffet();
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
		DXRelacher(pVertexBuffer);
		DXRelacher(pIndexBuffer);
		DXRelacher(pConstantBuffer);
		DXRelacher(pEffet);
		DXRelacher(pVertexLayout);
		DXRelacher(pSampleState);
	}

	

	void ImportedObject::InitEffet()
	{
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

} // namespace PM3D

