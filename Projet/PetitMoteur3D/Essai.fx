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

struct VS_Sortie
{
	float4 Pos : SV_Position;
	float4 Pos_init : POSITION;
	float3 Norm :    TEXCOORD0;
	float3 vDirLum : TEXCOORD1;
	float3 vDirCam : TEXCOORD2;
	float2 coordTex : TEXCOORD3;
};

Texture2D textureEntrees[8]; // la texture
Texture2D filter; // le filtre 
SamplerState SampleState; // l'état de sampling

VS_Sortie EssaiVS(float4 Pos : POSITION, float2 coordTex : TEXCOORD)
{
    VS_Sortie sortie = (VS_Sortie) 0;
    return sortie;
}


float4 EssaiPS(VS_Sortie vs) : SV_Target
{
    float3 couleur;

// Normaliser les paramètres
    float3 N = normalize(vs.Norm);
    float3 L = normalize(vs.vDirLum);
    float3 V = normalize(vs.vDirCam) / 1.2;

// Valeur de la composante diffuse
    float3 diff = saturate(dot(N, L));

// R = 2 * (N.L) * N – L
    float3 R = normalize(2 * diff * N - L);

// Puissance de 4 - pour l'exemple
    float S = pow(saturate(dot(R, V)), 4.0f);

// Échantillonner la couleur du pixel à partir de la texture
    float3 couleurTexture = textureEntrees[0].Sample(SampleState, vs.coordTex).rgb;

	// I = A + D * N.L + (R.V)n
    couleur = couleurTexture * vAEcl.rgb * vAMat.rgb +
			   couleurTexture * vDEcl.rgb * vDMat.rgb * diff;

    couleur += S;
	
    return float4(couleur, 1.0f);
}
struct GSPS_INPUT
{
    float4 Pos : SV_Position;
    float4 Pos_init : POSITION;
    float3 Norm : TEXCOORD0;
    float3 vDirLum : TEXCOORD1;
    float3 vDirCam : TEXCOORD2;
    float2 coordTex : TEXCOORD3;
};
 
[maxvertexcount(4)]
void EssaiGS(triangle GSPS_INPUT input[3],
              inout
TriangleStream<VS_Sortie> TriStream)
{
    VS_Sortie output;
     
    // Calculer la normale  
    float3 CoteA = input[1].Pos - input[0].Pos;
    float3 CoteB = input[2].Pos - input[0].Pos;
    float3 Normale = normalize(cross(CoteA, CoteB));
    float4 PointSupp = input[0].Pos + (input[2].Pos - input[0].Pos) / 2;
 
 // Points 0 et 1 
    for (int i = 0; i < 2; i++)
    {
        output.Pos = mul(input[i].Pos, matWorldViewProj);
        output.Norm = Normale;
        output.coordTex = input[i].coordTex;
        TriStream.Append(output);
    }
 
 // Point supplémentaire 
    output.Pos = mul(PointSupp, matWorldViewProj);
    output.Norm = Normale;
    output.coordTex = input[0].coordTex +
                     (input[2].coordTex - input[0].coordTex) / 2;
    TriStream.Append(output);
 
 // Point 2 
    output.Pos = mul(input[2].Pos, matWorldViewProj);
    output.Norm = Normale;
    output.coordTex = input[2].coordTex;
    TriStream.Append(output);
 
}

technique11 Essai
{
	pass pass0
	{
		SetVertexShader(CompileShader(vs_5_0,EssaiVS()));
        SetPixelShader(CompileShader(ps_5_0, EssaiPS()));
        SetGeometryShader(CompileShader(ps_5_0, EssaiGS()));
    }
}
