#pragma once

#include "DynamicObject.h"

using namespace DirectX;
namespace PM3D
{
	class CCamera {
	public:
		CCamera() {};

		bool is_free_cam{ false };

		float cameraSpeed = 0.3f;
		float scalePrev = 1.0f;
		
		XMVECTOR position;
		XMVECTOR rotation;
		XMVECTOR up;
		XMVECTOR down;
		XMFLOAT3 lookAtprev;

		XMVECTOR forward;
		XMVECTOR backward;
		XMVECTOR left ;
		XMVECTOR right;

		XMFLOAT3 pos; 
		XMFLOAT3 rot;

		XMMATRIX pMatView;
		XMMATRIX pMatProj;

		//Permet de passer de la premiere a la troisieme personne
		bool firstPerson = false;

		const XMVECTOR DEFAULT_FORWARD_VECTOR = XMVectorSet (0.0f, 0.0f, 1.0f, 1.0f); 
		const XMVECTOR DEFAULT_UP_VECTOR = XMVectorSet (0.0f, 1.0f, 0.0f, 1.0f);
		const XMVECTOR DEFAULT_DOWN_VECTOR = XMVectorSet(0.0f, -1.0f, 0.0f, 1.0f);

		const XMVECTOR DEFAULT_BACKWARD_VECTOR = XMVectorSet(0.0f, 0.0f, -1.0f, 1.0f);
		
		const XMVECTOR DEFAULT_LEFT_VECTOR = XMVectorSet(-1.0f, 0.0f, 0.0f, 1.0f);
		const XMVECTOR DEFAULT_RIGHT_VECTOR = XMVectorSet(1.0f, 0.0f, 0.0f, 1.0f);
		
		void init(bool free);

		void SetPosition(float x, float y ,float z );

		void SetRotation(float x, float y, float z);

		void adjustPosition(float x, float y, float z);

		void adjustPosition(const XMVECTOR& position_in);

		void adjustRotation(float x, float y, float z);

		void SetLookAtPos(XMFLOAT3 lookAtThis); 

		void update();


		//Anime les mouvements de la free cam
		void Anime(float tempsEcoule);
		void setProjection(float fov, float ratio, float nearZ , float farZ ); 


		//Suit un Objet de la scène
		void follow(DynamicObject& object); 
	};
} // namespace PM3D

