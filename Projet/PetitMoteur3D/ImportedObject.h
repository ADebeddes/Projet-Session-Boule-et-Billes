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
			
			-127.0f, 980.0f, -122.0f, 0.4f,
			8.0f, 912.0f, -76.0f, 0.4f,
			165.0f, 941.0f, -153.0f, 0.4f
		};
		
		

	public:
		ImportedObject(CDispositifD3D11* pDispositif_, string path, LPCWSTR fong_file = L"MiniPhong.fx", bool need_filter = false); 


		virtual ~ImportedObject();
		virtual void Draw() override;

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

		CDispositifD3D11* pDispositif;

		ID3D11Buffer* pVertexBuffer;
		ID3D11Buffer* pIndexBuffer;

		// Définitions des valeurs d'animation
		ID3D11Buffer* pConstantBuffer;
		
		
		// Pour les effets
		ID3DX11Effect* pEffet;
		ID3DX11EffectTechnique* pTechnique;
		ID3DX11EffectPass* pPasse;
		ID3D11InputLayout* pVertexLayout;

		ID3D11ShaderResourceView* maxY_view;
		std::vector <ID3D11ShaderResourceView*> resourcesView;  
		ID3D11ShaderResourceView* map_filter; 

		ID3D11SamplerState* pSampleState;

	
	};
};




