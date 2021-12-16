#include "stdafx.h"
#include "Physique.h"
// #include "PxPhysicsAPI.h"
// #include "cooking/PxCooking.h"
using namespace physx;
using namespace std;

namespace PM3D
{
	PxFilterFlags ProjectFilterShader(
		PxFilterObjectAttributes attributes0, PxFilterData filterData0,
		PxFilterObjectAttributes attributes1, PxFilterData filterData1,
		PxPairFlags& pairFlags, const void* constantBlock, PxU32 constantBlockSize)
	{
		PX_UNUSED(constantBlock); 
		PX_UNUSED(constantBlockSize);


		// let triggers through
		if (PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
		{
			pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
			return PxFilterFlag::eDEFAULT;
		}


		if ((filterData0.word0 == FilterGroup::boule && filterData1.word0 == FilterGroup::terrain) || (filterData1.word0 == FilterGroup::boule && filterData0.word0 == FilterGroup::terrain) ) {
			pairFlags = PxPairFlag::eCONTACT_DEFAULT | PxPairFlag::eNOTIFY_TOUCH_FOUND;
			return PxFilterFlag::eDEFAULT;
		}

		else if ((filterData0.word0 == FilterGroup::boule && filterData1.word0 == FilterGroup::colider) || (filterData1.word0 == FilterGroup::boule && filterData0.word0 == FilterGroup::colider)) {
			pairFlags = PxPairFlag::eTRIGGER_DEFAULT | PxPairFlag::eMODIFY_CONTACTS;
			return PxFilterFlag::eDEFAULT;
		}

		else if (
			(filterData0.word0 == FilterGroup::boule && filterData1.word0 == FilterGroup::boule) ||
			(filterData0.word0 == FilterGroup::boule && filterData1.word0 == FilterGroup::obstacle) || (filterData1.word0 == FilterGroup::boule && filterData0.word0 == FilterGroup::obstacle)) {
			pairFlags = PxPairFlag::eCONTACT_DEFAULT |  PxPairFlag::eSOLVE_CONTACT | PxPairFlag::eDETECT_DISCRETE_CONTACT | PxPairFlag::eNOTIFY_THRESHOLD_FORCE_FOUND;
			return PxFilterFlag::eDEFAULT;
		}


		return PxFilterFlag::eDEFAULT;
	}

	void Physique::initPhysics(bool interactive)
	{
		PX_UNUSED(interactive);

		gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
		gPvd = PxCreatePvd(*gFoundation);
		PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate(PVD_HOST, 5425, 10);




		gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);

		PxCudaContextManagerDesc cudaContextManagerDesc;

#ifdef RENDER_SNIPPET
		cudaContextManagerDesc.interopMode = PxCudaInteropMode::OGL_INTEROP;	//Choose interop mode. As the snippets use OGL, we select OGL_INTEROP
																				//when using D3D, cudaContextManagerDesc.graphicsDevice must be set as the graphics device pointer.
#else
		cudaContextManagerDesc.interopMode = PxCudaInteropMode::NO_INTEROP;
#endif


		gCudaContextManager = PxCreateCudaContextManager(*gFoundation, cudaContextManagerDesc, PxGetProfilerCallback());	//Create the CUDA context manager, required for GRB to dispatch CUDA kernels.
		if (gCudaContextManager)
		{
			if (!gCudaContextManager->contextIsValid())
			{
				gCudaContextManager->release();
				gCudaContextManager = NULL;
			}
		}
		if (!PxInitExtensions(*gPhysics, gPvd))
			exit(-1);


		PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
		sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
		gDispatcher = PxDefaultCpuDispatcherCreate(4);			//Create a CPU dispatcher using 4 worther threads
		sceneDesc.cpuDispatcher = gDispatcher;
		sceneDesc.filterShader = PxDefaultSimulationFilterShader;

		sceneDesc.cudaContextManager = gCudaContextManager;		//Set the CUDA context manager, used by GRB.

		sceneDesc.flags |= PxSceneFlag::eENABLE_GPU_DYNAMICS;	//Enable GPU dynamics - without this enabled, simulation (contact gen and solver) will run on the CPU.
		sceneDesc.flags |= PxSceneFlag::eENABLE_PCM;			//Enable PCM. PCM NP is supported on GPU. Legacy contact gen will fall back to CPU
		sceneDesc.flags |= PxSceneFlag::eENABLE_STABILIZATION;	//Improve solver stability by enabling post-stabilization.
		sceneDesc.broadPhaseType = PxBroadPhaseType::eGPU;		//Enable GPU broad phase. Without this set, broad phase will run on the CPU.
		sceneDesc.gpuMaxNumPartitions = 8;						//Defines the maximum number of partitions used by the solver. Only power-of-2 values are valid. 
																//A value of 8 generally gives best balance between performance and stability.

		sceneDesc.simulationEventCallback = &contact;
		sceneDesc.contactModifyCallback = &contact;
		sceneDesc.filterShader = ProjectFilterShader;


		gScene = gPhysics->createScene(sceneDesc);


		PxTolerancesScale toleranceScale;
		toleranceScale.length = 1000;
		toleranceScale.speed = sceneDesc.gravity.y;

		assert(toleranceScale.isValid());

		gCooking = PxCreateCooking(PX_PHYSICS_VERSION, gPhysics->getFoundation(), PxCookingParams(PxTolerancesScale()));
		if (!gCooking)
		{
			exit(-2);
		}


		PxPvdSceneClient* pvdClient = gScene->getScenePvdClient();
		if (pvdClient)
		{
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
			pvdClient->setScenePvdFlag(PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
		}
		gMaterial = gPhysics->createMaterial(1.0f, 0.40f, 0.00001f);
	}


	void Physique::cleanupPhysics(bool /*interactive*/)
	{
		PX_RELEASE(gScene);
		PX_RELEASE(gDispatcher);
		PX_RELEASE(gPhysics);

		if (gPvd)
		{
			PxPvdTransport* transport = gPvd->getTransport();
			gPvd->release();	gPvd = NULL;
			PX_RELEASE(transport);
		}

		PX_RELEASE(gCudaContextManager);
		PX_RELEASE(gFoundation);

		printf("done.\n");
	}

	void Physique::stepPhysics(bool /*interactive*/)
	{
		gScene->simulate(1.0f / 60.0f);
		gScene->fetchResults(true);
	}
}

