#pragma once
#include "StdAfx.h"
#include "Camera.h"
#include <dinput.h>
#include "MoteurWindows.h"


/*

credit to Jpres https://www.youtube.com/channel/UC5Lxe7GAsk_f8qMBsNmlOJg
for his videos on how to do a camera in DX11 !

*/

using namespace DirectX;
namespace PM3D

{
	void CCamera::init(bool free)
	{

		this->pos = XMFLOAT3(0.0f, 0.0f, 0.0f);
		this->lookAtprev = XMFLOAT3(0.0f,0.0f, 1.0f);
		this->position = XMLoadFloat3(&this->pos);
		this->rot = XMFLOAT3(0.0f, 0.0f, 0.0f);
		this->rotation = XMLoadFloat3(&this->pos);
		is_free_cam = free;


	}

	void CCamera::SetPosition(float x, float y, float z)
	{
		this->pos = XMFLOAT3(x, y, z);
		this->position = XMLoadFloat3(&this->pos);
		this->update();
	}

	void CCamera::SetRotation(float x, float y, float z)
	{
		this->rot = XMFLOAT3(x, y, z);
		this->rotation = XMLoadFloat3(&this->pos);
		this->update();
	}

	void CCamera::adjustPosition(float x, float y, float z)
	{
		this->pos.x += x;
		this->pos.y += y;
		this->pos.z += z;
		this->position = XMLoadFloat3(&this->pos);
		this->update();

	}

	void CCamera::adjustPosition(const XMVECTOR& position_in)
	{
		this->position += position_in;
		XMStoreFloat3(&this->pos, this->position);
		this->update();

	}

	void CCamera::adjustRotation(float x, float y, float z)
	{
		this->rot.x += x;
		this->rot.y += y;
		this->rot.z += z;
		this->rotation = XMLoadFloat3(&this->rot);
		this->update();

	}

	void CCamera::SetLookAtPos(XMFLOAT3 lookAtThis)
	{
		if (
			lookAtThis.x == this->pos.x &&
			lookAtThis.y == this->pos.y &&
			lookAtThis.z == this->pos.z
			)
		{
			return;
		}

		lookAtThis.x = this->pos.x - lookAtThis.x;
		lookAtThis.y = this->pos.y - lookAtThis.y;
		lookAtThis.z = this->pos.z - lookAtThis.z;


		float pitch{ 0.0f };
		if (lookAtThis.y != 0.0f)
		{
			const float distance = sqrt(lookAtThis.x * lookAtThis.x + lookAtThis.z * lookAtThis.z);
			pitch = atan(lookAtThis.y / distance);
		}

		float yaw{ 0.0f };
		if (lookAtThis.x != 0.0f)
		{
			yaw = atan(lookAtThis.x / lookAtThis.z);

		}
		if (lookAtThis.z > 0.0f)
		{
			yaw += XM_PI;
		}

		this->SetRotation(pitch, yaw, 0.0f);
		update();

	}

	void CCamera::update()
	{
		XMMATRIX camRotation = XMMatrixRotationRollPitchYaw(
			this->rot.x,
			this->rot.y,
			this->rot.z
		);
		XMVECTOR camTarget = XMVector3TransformCoord(
			this->DEFAULT_FORWARD_VECTOR,
			camRotation
		);
		camTarget += this->position;


		XMVECTOR upDirection = XMVector3TransformCoord(
			this->DEFAULT_UP_VECTOR,
			camRotation
		);


		this->pMatView = XMMatrixLookAtLH(
			this->position,
			camTarget,
			upDirection
		);

		XMMATRIX rotationMatrix{}; 
		
		rotationMatrix = XMMatrixRotationRollPitchYaw(this->rot.x, this->rot.y, this->rot.z);

		this->forward = XMVector3TransformCoord
		(
			this->DEFAULT_FORWARD_VECTOR,
			rotationMatrix
		);
		this->backward = XMVector3TransformCoord
		(
			this->DEFAULT_BACKWARD_VECTOR,
			rotationMatrix
		);
		this->left = XMVector3TransformCoord
		(
			this->DEFAULT_LEFT_VECTOR,
			rotationMatrix
		);
		this->right = XMVector3TransformCoord
		(
			this->DEFAULT_RIGHT_VECTOR,
			rotationMatrix
		);

		this->up = XMVector3TransformCoord
		(
			this->DEFAULT_UP_VECTOR,
			rotationMatrix
		);
		this->down = XMVector3TransformCoord
		(
			this->DEFAULT_DOWN_VECTOR,
			rotationMatrix
		);

	}
	void CCamera::Anime(float posMapY)
	{
		posMapY;

		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		CDIManipulateur& rGestionnaireDeSaisie =
			rMoteur.GetGestionnaireDeSaisie();

	

		// Vérifier l’état de la touche gauche 
		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_A))
		{
			adjustPosition(left * cameraSpeed);
		}

		// Vérifier l’état de la touche droite 
		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_D))
		{
			adjustPosition(right * cameraSpeed);
		}

		// Vérifier l’état de la touche haut
		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_W))
		{
			adjustPosition(forward * cameraSpeed);
		}

		// Vérifier l’état de la touche bas 
		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_S))
		{
			adjustPosition(backward * cameraSpeed);
		}

		// Vérifier l’état de la touche espace 
		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_SPACE))
		{
			adjustPosition(up * cameraSpeed);
		}

		// Vérifier l’état de la touche controle gauche 
		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_LCONTROL))
		{
			adjustPosition(down * cameraSpeed);
		}

		// Augmenter la vitesse de la caméra
		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_1))
		{
			cameraSpeed = cameraSpeed > 10.0f ? 10.0f : cameraSpeed + 0.01f;
		}

		// Diminuer la vitesse de la caméra
		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_2))
		{

			cameraSpeed = cameraSpeed < 0.01f ? 0.01f : cameraSpeed - 0.01f;

		}

		if ((rGestionnaireDeSaisie.EtatSouris().rgbButtons[0] & 0x80) &&
			(rGestionnaireDeSaisie.EtatSouris().lX < 0))
		{
			adjustRotation(0.0f, -0.1f, 0.0f);
		}


		if ((rGestionnaireDeSaisie.EtatSouris().rgbButtons[0] & 0x80) &&
			(rGestionnaireDeSaisie.EtatSouris().lX > 0))
		{
			adjustRotation(0.0f, 0.1f, 0.0f);
		}
		

		if ((rGestionnaireDeSaisie.EtatSouris().rgbButtons[0] & 0x80) &&
			(rGestionnaireDeSaisie.EtatSouris().lY < 0))
		{
			adjustRotation(-0.1f, 0.0f, 0.0f);
		}


		if ((rGestionnaireDeSaisie.EtatSouris().rgbButtons[0] & 0x80) &&
			(rGestionnaireDeSaisie.EtatSouris().lY > 0))
		{
			adjustRotation(0.1f, 0.0f, 0.0f);
		}

		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_F1))
		{
			is_free_cam = true;
		}

		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_F2))
		{
			is_free_cam = false;
		}


		if (!is_free_cam)

		{
			if (abs(posMapY - this->pos.y) > 2)
			{
				adjustPosition(0.0f, posMapY + 4.0f - pos.y, 0.0f);
			}
			else
			{
				adjustPosition(0.0f, 0.0f, 0.0f);
			}

		}

	

		update();
	}
	void CCamera::setProjection(float fov, float ratio, float nearZ, float farZ)
	{
		float fovRadians = (fov / 360.0f) * XM_2PI;
		this->pMatProj = XMMatrixPerspectiveFovLH(fovRadians, ratio, nearZ, farZ);
	}
	void CCamera::follow(DynamicObject& object)
	{
	
		const auto objectPos = object.body->getGlobalPose().p;
	
		const auto lookat  = object.body->getLinearVelocity().getNormalized();
		
		const auto incrementX = (lookat.x - lookAtprev.x) / 100;
		const auto incrementY = (lookat.y - lookAtprev.y) / 100;
		const auto incrementZ = (lookat.z - lookAtprev.z) / 100;

		lookAtprev = XMFLOAT3(lookAtprev.x + incrementX, lookAtprev.y + incrementY, lookAtprev.z + incrementZ);

		const auto scale = object.scale;

		const auto scaleInc = (scale - scalePrev) / 30;

		scalePrev += scaleInc;

		if (!firstPerson) {
			SetPosition(objectPos.x - lookAtprev.x * 4*scalePrev , objectPos.y - lookAtprev.y * 8* scalePrev + 2, objectPos.z - lookAtprev.z * 4 * scalePrev);
		}
		else {
			SetPosition(objectPos.x, objectPos.y, objectPos.z);

		}
		SetLookAtPos(XMFLOAT3(objectPos.x +lookAtprev.x, objectPos.y + lookAtprev.y, objectPos.z + lookAtprev.z ));

		
	}
}// namespace PM3D