////////////////////////////////////////////
////////Shader variables 
////////////////////////////////////////////
//global var
float textureReplication = 50.0f;
float vertexDisplacementForce = 3.0f;
Texture2D textureEntrees[8]; // la texture
Texture2D filter; // le filtre 
SamplerState SampleState; // l'état de sampling



////////////////////////////////////////////
////////Shader Params
////////////////////////////////////////////
cbuffer param
{
	float4x4 matWorldViewProj;   // la matrice totale 
	float4x4 matWorld;		// matrice de transformation dans le monde 
	float4 vLumiere; 		// la position de la source d'éclairage (Point)
	float4 vCamera; 			// la position de la caméra
	float4 vAEcl; 			// la valeur ambiante de l'éclairage
	float4 vAMat; 			// la valeur ambiante du matériau
	float4 vDEcl; 			// la valeur diffuse de l'éclairage 
	float4 vDMat; 			// la valeur diffuse du matériau 
}

////////////////////////////////////////////
////////Vertex Shader 
////////////////////////////////////////////
struct VS_Sortie
{
	float4 Pos : SV_Position;
	float4 Pos_init : POSITION;
	float3 Norm :    TEXCOORD0;
	float3 vDirLum : TEXCOORD1;
	float3 vDirCam : TEXCOORD2;
	float2 coordTex : TEXCOORD3;
};

VS_Sortie MiniPhongVS(float4 Pos : POSITION, float3 Normale : NORMAL, float2 coordTex : TEXCOORD)
{
    VS_Sortie sortie = (VS_Sortie) 0;
	
    float4 textureDisplacement = float4(textureEntrees[3].SampleLevel(SampleState, coordTex * textureReplication, 0).rgb, 0.0f);

    sortie.Pos_init = Pos - textureDisplacement * vertexDisplacementForce;
    sortie.Pos = mul(sortie.Pos_init, matWorldViewProj);
    sortie.Norm = mul(float4(Normale, 0.0f), matWorld).xyz;

    float3 PosWorld = mul(sortie.Pos, matWorld).xyz;

    sortie.vDirLum = vLumiere.xyz - PosWorld;
    sortie.vDirCam = vCamera.xyz - PosWorld;

	// Coordonnées d'application de texture
    sortie.coordTex = coordTex;

    return sortie;
}


////////////////////////////////////////////
////////Geometry Shader 
////////////////////////////////////////////
//struct GSPS_INPUT
//{
//    float4 Pos : SV_POSITION;
//    float3 Norm : TEXCOORD0;
//    float2 Tex : TEXCOORD1;
//};

//[maxvertexcount(12)]
//void MiniPhongGS(triangle VS_Sortie input[3],
//inout TriangleStream<VS_Sortie> TriStream)
//{
//    VS_Sortie output;
	
	
//}



////////////////////////////////////////////
////////Pixel Shader 
////////////////////////////////////////////
float4 NulPS(VS_Sortie vs) : SV_Target
{
    float4 couleur;

    couleur = textureEntrees[1].Sample(SampleState, vs.coordTex);

    return couleur;
}

float4 MiniPhongPS(VS_Sortie vs) : SV_Target
{
	//local variables
    float3 couleur = 0;
    float3 bumpMap_Wall = textureEntrees[2].Sample(SampleState, vs.coordTex * textureReplication).rgb;
    
	
	
	// light params normal pixel, light vector, view vector
    float3 N = normalize(bumpMap_Wall);	//bumpMapping texture
	float3 L = normalize(vs.vDirLum);
	float3 V = normalize(vs.vDirCam) * 0.8; // 0.8 apply a less plastic effect

	// Valeur de la composante diffuse
	float3 diff = saturate(dot(N, L));

	// R = 2 * (N.L) * N – L
	float3 R = normalize(2 * diff * N - L);

	// Puissance de 4 - pour l'exemple
	float S = pow(saturate(dot(R, V)), 8.0f);

	// Échantillonner la couleur du pixel à partir de la texture
	float3 couleurTexture;

	float3 couleurTexture_neige = textureEntrees[0].Sample(SampleState, vs.coordTex * 12).rgb; 
	float3 couleurTexture_Wall = textureEntrees[1].Sample(SampleState, vs.coordTex * textureReplication).rgb;
	
	couleurTexture = couleurTexture_Wall;

	// I = A + D * N.L + (R.V)n
	couleur = couleurTexture * vAEcl.rgb * vAMat.rgb + couleurTexture * vDEcl.rgb * vDMat.rgb * diff;

	couleur += S;

	return float4(couleur, 1.0f);
}


technique11 Nul
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_5_0, MiniPhongVS()));
        SetPixelShader(CompileShader(ps_5_0, NulPS()));
        SetGeometryShader(NULL);
    }
}

technique11 MiniPhong
{
	pass pass0
	{
		SetVertexShader(CompileShader(vs_5_0, MiniPhongVS()));
		SetPixelShader(CompileShader(ps_5_0, MiniPhongPS()));
        //SetGeometryShader(CompileShader(gs_5_0, MiniPhongGS()));
        SetGeometryShader(NULL);
    }
}


int nbTextureUsed = 3; // dont 1 normalTexture