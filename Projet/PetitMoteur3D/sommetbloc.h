
#pragma once
using namespace DirectX;

namespace PM3D
{
/// Classe : CSommetBloc 
/// 
/// But : contient les information necessaire a la creation d'objet 3D
/// 	  definie un vertex, sa normale associée et une coordonnée de texture
/// 
class CSommetBloc
{
public:
	CSommetBloc() = default;
	CSommetBloc(const XMFLOAT3& position, const XMFLOAT3& normal, const XMFLOAT2& coordTex = XMFLOAT2(0.0f, 0.0f));

	static UINT numElements;
	static D3D11_INPUT_ELEMENT_DESC layout[];

private:
	XMFLOAT3 m_Position;
	XMFLOAT3 m_Normal;
	XMFLOAT2 m_CoordTex;
};

} // namespace PM3D
