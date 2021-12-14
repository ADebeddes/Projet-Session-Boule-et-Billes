#include "stdafx.h"
#include "CameraManager.h"
#include "MoteurWindows.h"

namespace PM3D {
	void CameraManager::switchCamera()
	{
		CMoteurWindows& rMoteur = CMoteurWindows::GetInstance();
		CDIManipulateur& rGestionnaireDeSaisie =
			rMoteur.GetGestionnaireDeSaisie();

		//Pour passer en caméra libre ( Pour le dev)
		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_F)) {
			cameras[0].SetPosition(XMVectorGetX(cameras[cameraActive].position), XMVectorGetY(cameras[cameraActive].position), XMVectorGetZ(cameras[cameraActive].position));
			cameras[0].SetRotation(cameras[cameraActive].rot.x, cameras[cameraActive].rot.y, cameras[cameraActive].rot.z);
			cameraActive = 0;

		}


		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_G)) {
			cameraActive = 1;
		}

		if (rGestionnaireDeSaisie.ToucheAppuyee(DIK_H)) {
			cameraActive = 2;
		}
	}
	void CameraManager::addCamera(CCamera cam)
	{
		cameras.emplace(std::make_pair(static_cast<int>(cameras.size()), cam));
	}
	void CameraManager::Anime(DynamicObject& object, float hauteur)
	{
		cameras[0].Anime(hauteur);
		cameras[1].follow(object);
		cameras[2].follow(object);

	}
	CCamera CameraManager::getActive()
	{
		return cameras[cameraActive];
	}
	CameraManager::CameraManager()
	{
		cameras.emplace(std::make_pair(0, CCamera()));
		cameras.emplace(std::make_pair(1, CCamera()));
		cameras.emplace(std::make_pair(2, CCamera()));
		cameras[0].init(true);
		cameras[1].init(true);
		cameras[1].firstPerson = true;
		cameras[2].init(true);
		cameras[2].firstPerson = false;
	}
	void CameraManager::setProjection(float fov, float ratio, float nearZ, float farZ)
	{
		cameras[0].setProjection( fov,  ratio,  nearZ,  farZ);
		cameras[1].setProjection( fov,  ratio,  nearZ,  farZ);
		cameras[2].setProjection( fov,  ratio,  nearZ,  farZ);
	}
}