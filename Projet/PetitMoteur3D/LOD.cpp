#include "stdafx.h"
#include "LOD.h"
namespace PM3D {
    bool LOD::afficherFaceCamera(const CCamera& cam, XMFLOAT3 pos,int nbSapin)
    {
        if (pos.x == -100 && pos.y == -100 && pos.z == -100) {
            return true;
        }
        auto posCam = cam.pos;
        auto vecCamObj = XMVectorSet(pos.x - posCam.x, pos.y - posCam.y, pos.z - posCam.z, 1.0f);
        auto dirCam = cam.forward;

        auto dot = XMVectorGetX(vecCamObj) * XMVectorGetX(dirCam) + XMVectorGetY(vecCamObj) * XMVectorGetY(dirCam) + XMVectorGetZ(vecCamObj) * XMVectorGetZ(dirCam);
        if (dot > 0) {
            auto disObj = sqrt(XMVectorGetX(vecCamObj) * XMVectorGetX(vecCamObj) + XMVectorGetY(vecCamObj) * XMVectorGetY(vecCamObj) + XMVectorGetZ(vecCamObj) * XMVectorGetZ(vecCamObj));
            if (disObj < 2000 - nbSapin * 5) {
                return true;
            }
            else {
                return false;
            }
        }
        else {
            return false;
        }

    }
}
