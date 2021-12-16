#pragma once

#include "d3dx11effect.h"
#include "Objet3D.h"
#include "Bloc.h"   // Pour l'index du bloc

#include "Texture.h"

#include <array>
#include <iostream>
#include <type_traits>
#include <fstream>
#include <string>
#include <vector>
#include "sommetbloc.h"
#include "ObjectImporter.h"
#include "PxPhysicsAPI.h"
using namespace physx; 
using namespace DirectX;
using namespace std; 
namespace PM3D
{
	//Notre importeur de Modèles
	class ImportedObject : public CObjet3D
	{
		DirectX::XMMATRIX lights_DL_PtL = {
			300.0f, 400.0f, 100.0f, 1.0f, //directionnal light
			-127.0f, 980.0f, -122.0f, 0.0f,
			8.0f, 912.0f, -76.0f, 0.0f,
			165.0f, 941.0f, -153.0f, 0.0f
		};

		struct MaterialBlock
		{
			char NomFichierTexture[200];
			char NomMateriau[60];
			XMFLOAT4 Ambient;
			XMFLOAT4 Diffuse;
			XMFLOAT4 Specular;
			float Puissance;
			bool transparent;
		};

		struct CMaterial
		{
			std::string NomFichierTexture;
			std::string NomMateriau;
			ID3D11ShaderResourceView* pTextureD3D;

			XMFLOAT4 Ambient;
			XMFLOAT4 Diffuse;
			XMFLOAT4 Specular;
			float Puissance;
			bool transparent;

			CMaterial()
				: pTextureD3D(nullptr)
				, Ambient(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f))
				, Diffuse(XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f))
				, Specular(XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f))
				, Puissance(1.0f)
				, transparent(false)
			{
			}

			void MatToBlock(MaterialBlock& mb)
			{
				strcpy_s(mb.NomFichierTexture, NomFichierTexture.c_str());
				strcpy_s(mb.NomMateriau, NomMateriau.c_str());
				mb.Ambient = Ambient;
				mb.Diffuse = Diffuse;
				mb.Specular = Specular;
				mb.Puissance = Puissance;
				mb.transparent = transparent;

			}

			void BlockToMat(MaterialBlock& mb)
			{
				NomFichierTexture.append(mb.NomFichierTexture);
				NomMateriau.append(mb.NomMateriau);
				Ambient = mb.Ambient;
				Diffuse = mb.Diffuse;
				Specular = mb.Specular;
				Puissance = mb.Puissance;
				transparent = mb.transparent;
			}
		};
		
		

	public:
		ImportedObject(CDispositifD3D11* pDispositif_, string path, LPCWSTR fong_file = L"MiniPhong.fx", bool need_filter = false, bool shadow_ok = false);


		virtual ~ImportedObject();
		virtual void Draw() override;

		void DrawShadows();

		void SetTexture(CTexture* pTexture);

		void SetFilter(CTexture* pTexture);

		float getHeightAt(float x, float z);

		virtual void InitPhysique(char*) {};

		virtual void UpdatePhysique(float) {};

		bool has_texture { false };
		bool need_filter { false };
		LPCWSTR fong_file;
 
		ObjectImporter oi;

		float size_x;
		float size_y;
		float size_z;
		XMMATRIX matWorld;

		PxRigidActor* body;

	protected:

		vector<XMFLOAT3> points;
		CSommetBloc* sommets;
		unsigned int* index;

		unsigned int index_size;
		int sommets_size;

		void InitEffet();

		void InitEffetShadows();

		void InitMatricesShadowMap();

		void InitDepthBuffer();

		CDispositifD3D11* pDispositif;

		ID3D11Buffer* pVertexBuffer;
		ID3D11Buffer* pIndexBuffer;

		// Définitions des valeurs d'animation
		ID3D11Buffer* pConstantBuffer;

		std::vector<CMaterial> Material;
		
		
		// Pour les effets
		ID3DX11Effect* pEffet;
		ID3DX11EffectTechnique* pTechnique;
		ID3DX11EffectPass* pPasse;
		ID3D11InputLayout* pVertexLayout;

		ID3D11ShaderResourceView* maxY_view;
		std::vector <ID3D11ShaderResourceView*> resourcesView;  
		ID3D11ShaderResourceView* map_filter; 

		static const int SHADOWMAP_DIM = 512;
		
		ID3D11InputLayout* pVertexLayoutShadow;
		ID3D11Texture2D* pTextureShadowMap;         // Texture pour le shadow map
		ID3D11RenderTargetView* pRenderTargetView;  // Vue cible de rendu
		ID3D11ShaderResourceView* pShadowMapView;   // Vue ressource de shader
		ID3D11Texture2D* pDepthTexture;				// texture de profondeur
		ID3D11DepthStencilView* pDepthStencilView;  // Vue tampon de profondeur

		ID3D11SamplerState* pSampleState;

		XMMATRIX mVLight;
		XMMATRIX mPLight;
		XMMATRIX mVPLight;

	
	};
};




