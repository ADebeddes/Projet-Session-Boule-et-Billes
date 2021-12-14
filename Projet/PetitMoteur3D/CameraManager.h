#pragma once
#include <unordered_map>
#include "Camera.h"

namespace PM3D {

/// Classe : CameraManager 
/// 
/// But : gere l'ensemble des cameras et leur comportement
/// 	  permet l'utilisation de plusieurs cameras
///
	class CameraManager
	{
	public:

		std::unordered_map<int, CCamera> cameras;
		int cameraActive = 2;

		void switchCamera();
		void addCamera(CCamera cam);
		void Anime(DynamicObject& object, float hauteur);
		
		CCamera getActive();

		CameraManager();
		void setProjection(float fov, float ratio, float nearZ, float farZ);


	};
}

