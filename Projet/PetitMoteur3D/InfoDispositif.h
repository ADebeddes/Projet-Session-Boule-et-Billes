#pragma once

namespace PM3D
{

enum INFODISPO_TYPE
{
	ADAPTATEUR_COURANT
};

class CInfoDispositif
{
public:

	explicit CInfoDispositif(int NoAdaptateur);
	explicit CInfoDispositif(DXGI_MODE_DESC modeDesc);

	void GetDesc(DXGI_MODE_DESC& modeDesc) const noexcept { modeDesc = mode; }

private:
	bool valide;
	int largeur = 0;
	int hauteur = 0;
	int memoire = 0;
	wchar_t nomcarte[128] = L"\0";
	DXGI_MODE_DESC mode;
};

} // namespace PM3D
