#include "stdafx.h"
#include "Skybox.h"
#include "resource.h"
#include "MoteurWindows.h"

Skybox::Skybox(CDispositifD3D11* pDispositif_, string path, char* name)

	: ImportedObject(pDispositif_, path,L"Skybox.fx",false, false)
		, object_name{ name }
	{
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		SetTexture(rMoteur.GetTextureManager().GetNewTexture(L"skybox6.dds", pDispositif));
		
	}




void Skybox::Anime(float tempsEcoule)
{
	CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();

	const auto tm = rMoteur.pEntityManager->pPlayer->playerCharacter.body->getGlobalPose();
	
	matWorld = XMMatrixScaling(10000, 10000, 10000);
	matWorld = matWorld * XMMatrixRotationAxis(XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f), XMVectorGetX(g_XMHalfPi));
	matWorld = matWorld * XMMatrixTranslation(tm.p.x, tm.p.y, tm.p.z);


}
struct ShadersParams
{
	XMMATRIX matWorldViewProj;	// la matrice totale 
	XMMATRIX matWorld;			// matrice de transformation dans le monde 
	XMVECTOR vLumiere; 			// la position de la source d'éclairage (Point)
	XMVECTOR vCamera; 			// la position de la caméra
	XMVECTOR vAEcl; 			// la valeur ambiante de l'éclairage
	XMVECTOR vAMat; 			// la valeur ambiante du matériau
	XMVECTOR vDEcl; 			// la valeur diffuse de l'éclairage 
	XMVECTOR vDMat; 			// la valeur diffuse du matériau 
};
void Skybox::Draw()
{
	pDispositif->CullingInv();
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

	sp.vLumiere = XMVectorSet(300.0f, 1000.0f, 100.0f, 1.0f);
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
	pDispositif->CullingNormal();

}
