#include "StdAfx.h"
#include "InfoDispositif.h"

#include "util.h"

namespace PM3D
{

// Pour obtenir les informations � partir d'un num�ro d'adaptateur
// 0 = d�faut = ADAPTATEUR_DE_DEFAUT
CInfoDispositif::CInfoDispositif(int NoAdaptateur)
{
	IDXGIFactory* pFactory = nullptr;
	//juste pour enlever le warning
	if (pFactory == nullptr)pFactory = nullptr;
	//juste pour enlever le warning
	IDXGIAdapter* pAdapter = nullptr;
	if (pAdapter == nullptr)pAdapter = nullptr;
	//juste pour enlever le warning
	IDXGIOutput* pOutput = nullptr;
	if (pOutput == nullptr)pAdapter = nullptr;

	valide = false;

	// Cr�er un IDXGIFactory.
	CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&pFactory));

	if (FAILED(pFactory->EnumAdapters(NoAdaptateur, &pAdapter))) return;

	// Obtenir les informations de la premi�re sortie de l'adaptateur 
	// (Le moniteur)
	if (FAILED(pAdapter->EnumOutputs(0, &pOutput))) return;

	// Obtenir la description de l'�tat courant
	DXGI_OUTPUT_DESC outDesc;
	pOutput->GetDesc(&outDesc);

	largeur = outDesc.DesktopCoordinates.right - outDesc.DesktopCoordinates.left;
	hauteur = outDesc.DesktopCoordinates.bottom - outDesc.DesktopCoordinates.top;
	valide = true;

	DXGI_ADAPTER_DESC Desc;
	pAdapter->GetDesc(&Desc);

	// M�moire d�di�e (en megabytes).
	memoire = int(Desc.DedicatedVideoMemory / 1024 / 1024);

	// Nom de la carte video.
	wcscpy_s(nomcarte, 128, Desc.Description);

	// Faire le m�nage pour �viter les �memory leaks�
	DXRelacher(pOutput);
	DXRelacher(pAdapter);
	DXRelacher(pFactory);
}

CInfoDispositif::CInfoDispositif(DXGI_MODE_DESC modeDesc)
{
	// �num�ration des adaptateurs
	IDXGIFactory * pFactory = nullptr;
	if (pFactory == nullptr)pFactory = nullptr;

	CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)(&pFactory));

	IDXGIAdapter * pAdapter = nullptr;
	if (pAdapter == nullptr)pAdapter = nullptr;

	std::vector <IDXGIAdapter*> vAdapters;

	for (UINT i = 0;
		pFactory->EnumAdapters(i, &pAdapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		vAdapters.push_back(pAdapter);
	}

	// On travaille presque toujours avec vAdapters[0] 
	// � moins d'avoir plusieurs cartes non-collaboratives
	*this = CInfoDispositif(0);

	// Obtenir la sortie 0 - le moniteur principal
	IDXGIOutput* pOutput = nullptr;
	if (pOutput == nullptr)pOutput = nullptr;

	//vAdapters[0]->EnumOutputs(0, &pOutput);
	vAdapters.at(0)->EnumOutputs(0, &pOutput);

	// Obtenir le mode le plus int�ressant
	pOutput->FindClosestMatchingMode(&modeDesc, &mode, nullptr);

	// Faire le m�nage pour �viter les �memory leaks�
	DXRelacher(pOutput);

	for (int i = 0; i < vAdapters.size(); ++i)
	{
		DXRelacher(vAdapters.at(i));
	}

	DXRelacher(pFactory);
}

} // namespace PM3D
