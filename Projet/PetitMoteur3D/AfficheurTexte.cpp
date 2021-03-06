#include "StdAfx.h"
#include "AfficheurTexte.h"
#include "util.h"


namespace PM3D
{

ULONG_PTR CAfficheurTexte::token = 0;

CAfficheurTexte::CAfficheurTexte(CDispositifD3D11* pDispositif, int largeur, int hauteur, Gdiplus::Font* pPolice,bool _fond)
	: pDispo(pDispositif)
	, TexWidth(largeur)
	, TexHeight(hauteur)
	, pFont(pPolice)
	, pTexture(nullptr)
	, pSurface(nullptr)
	, pTextureView(nullptr)
	, pCharBitmap(nullptr)
	, pCharGraphics(nullptr)
	, pBlackBrush(nullptr)
	, fond(_fond)
{
	// Cr?er le bitmap et un objet GRAPHICS (un dessinateur)
	pCharBitmap = std::make_unique<Gdiplus::Bitmap>(TexWidth, TexHeight, PixelFormat32bppARGB);
	pCharGraphics = std::make_unique<Gdiplus::Graphics>(pCharBitmap.get());

	// Param?tres de l'objet Graphics
	pCharGraphics->SetCompositingMode(Gdiplus::CompositingModeSourceOver);
	pCharGraphics->SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed);
	pCharGraphics->SetInterpolationMode(Gdiplus::InterpolationModeHighQuality);
	pCharGraphics->SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighSpeed);
	pCharGraphics->SetSmoothingMode(Gdiplus::SmoothingModeNone);
	pCharGraphics->SetPageUnit(Gdiplus::UnitPixel);
	Gdiplus::TextRenderingHint hint = Gdiplus::TextRenderingHintAntiAlias; //TextRenderingHintSystemDefault;  
	pCharGraphics->SetTextRenderingHint(hint);

	// Un brosse noire pour le remplissage
	//		Notez que la brosse aurait pu ?tre pass?e 
	//		en param?tre pour plus de flexibilit?
	pBlackBrush = std::make_unique<Gdiplus::SolidBrush>(Gdiplus::Color(255, 255, 255, 255));

	// On efface le bitmap (notez le NOIR TRANSPARENT...)
	pCharGraphics->Clear(Gdiplus::Color(0, 0, 0, 0));

	// Acc?der aux bits du bitmap
	Gdiplus::BitmapData bmData;
	pCharBitmap->LockBits(&Gdiplus::Rect(0, 0, TexWidth, TexHeight), Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmData);

	// Cr?ation d'une texture de m?me dimension sur la carte graphique
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = TexWidth;
	texDesc.Height = TexHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = bmData.Scan0;
	data.SysMemPitch = TexWidth * 4;
	data.SysMemSlicePitch = 0;

	// Cr?ation de la texture ? partir des donn?es du bitmap
	DXEssayer(pDispo->GetD3DDevice()->CreateTexture2D(&texDesc, &data, &pTexture));

	// Cr?ation d'un ?resourve view? pour acc?der facilement ? la texture
	//     (comme pour les sprites)
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;

	DXEssayer(pDispositif->GetD3DDevice()->CreateShaderResourceView(pTexture, &srvDesc, &pTextureView));

	pCharBitmap->UnlockBits(&bmData);
}

void CAfficheurTexte::Init()
{
	Gdiplus::GdiplusStartupInput  startupInput(0, TRUE, TRUE);
	Gdiplus::GdiplusStartupOutput startupOutput;

	GdiplusStartup(&token, &startupInput, &startupOutput);
}

void CAfficheurTexte::Close()
{
	Gdiplus::GdiplusShutdown(token);
}

CAfficheurTexte::~CAfficheurTexte()
{
	DXRelacher(pTextureView);
	DXRelacher(pSurface);
	DXRelacher(pTexture);
}

void CAfficheurTexte::Ecrire(const std::wstring& s)
{
	// Effacer
	if (fond) {
		pCharGraphics->Clear(Gdiplus::Color(150, 0, 200, 255));
	}
	else {
		pCharGraphics->Clear(Gdiplus::Color(0, 0, 0, 0));
	}
	

	// ?crire le nouveau texte
	pCharGraphics->DrawString(s.c_str(), static_cast<int>(s.size()), pFont, Gdiplus::PointF(0.0f, 0.0f), pBlackBrush.get());

	// Transf?rer
	Gdiplus::BitmapData bmData;
	pCharBitmap->LockBits(&Gdiplus::Rect(0, 0, TexWidth, TexHeight), Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &bmData);

	pDispo->GetImmediateContext()->UpdateSubresource(pTexture, 0, 0, bmData.Scan0, TexWidth * 4, 0);

	pCharBitmap->UnlockBits(&bmData);
}

} // namespace PM3Ds
