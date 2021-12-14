#include "stdafx.h"
#include "dispositif.h"

namespace PM3D
{

void CDispositif::Present()
{
	PresentSpecific();
}

void CDispositif::setLargeur(int l)
{
	largeurEcran = l;
}

void CDispositif::setHauteur(int h)
{
	hauteurEcran = h;
}
} // namespace PM3D
