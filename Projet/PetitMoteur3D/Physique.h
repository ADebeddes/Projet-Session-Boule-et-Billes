#pragma once
#include <ctype.h>
#include <chrono>
#include <iostream>
#include <vector>
#include "PxPhysicsAPI.h"
#include "Contact.h"

#ifndef PHYSX_SNIPPET_PVD_H
#define PHYSX_SNIPPET_PVD_H

#define PVD_HOST "127.0.0.1"    //Set this to the IP address of the system running the PhysX Visual Debugger that you want to connect to.

#endif //PHYSX_SNIPPET_PVD_H

#define PX_RELEASE(x)if(x) { x->release();x =NULL; }

using namespace physx;
using namespace std;
namespace PM3D
{
    //Nos filtres de collision
    struct FilterGroup
    {

        enum Enum {
            boule = (1 << 2),
            obstacle = (1 << 3),
            checkpoint = (1 << 4),
            rien = (1 << 5),
            terrain = (1 << 6),
            colider = (1 << 7)

        };
    };

    //Notre classe faisant l'interface avec Physx
    class Physique
    {


    public:
        PxDefaultAllocator        gAllocator;
        PxDefaultErrorCallback    gErrorCallback;

        PxFoundation* gFoundation = NULL;

        PxDefaultCpuDispatcher* gDispatcher = NULL;

        PxPvd* gPvd = NULL;

        Contact contact;

        PxPhysics* gPhysics = NULL;

        PxScene* gScene = NULL;

        PxMaterial* gMaterial = NULL;

        PxCooking* gCooking;

        PxCudaContextManager* gCudaContextManager = NULL;

        void initPhysics(bool interactive);

        void stepPhysics(bool /*interactive*/);

        void cleanupPhysics(bool /*interactive*/);

    };

}