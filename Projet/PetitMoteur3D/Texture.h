#pragma once

namespace PM3D
{
/// Classe : CTexture 
/// 
/// But : contient une image de texture
/// 	  utilisée pour eviter la lecture de fichier image recurante
/// 
class CTexture
{
public:
	CTexture(const std::wstring& filename, CDispositifD3D11* pDispositif);
	~CTexture();

	const std::wstring& GetFilename() const { return m_Filename; }
	ID3D11ShaderResourceView* GetD3DTexture() { return m_Texture; }

private:
	std::wstring m_Filename;				//nom du fichier texture
	ID3D11ShaderResourceView* m_Texture;	//la texture en memoire 
};

} // namespace PM3D
