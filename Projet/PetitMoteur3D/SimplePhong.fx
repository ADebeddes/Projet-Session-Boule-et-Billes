//light variables
float mainLightInfluence = 1.0f;
float pointLightInfluence = 0.25;


cbuffer param
{
	float4x4 matWorldViewProj;   // la matrice totale 
	float4x4 matWorld;		// matrice de transformation dans le monde 
	float4x4 vLumiere; 		// la position de la source d'éclairage (Point)
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

	float3 PosLumPt1 : POSITION1;
	float3 PosLumPt2 : POSITION2;
	float3 PosLumPt3 : POSITION3;
	float3 vDirLumPt1 : TEXCOORD4;
	float3 vDirLumPt2 : TEXCOORD5;
	float3 vDirLumPt3 : TEXCOORD6;
	float intensityLum1 : FLOAT;
	float intensityLum2 : FLOAT1;
	float intensityLum3 : FLOAT2;

	float distL1 : FLOAT3;
	float distL2 : FLOAT4;
	float distL3 : FLOAT5;
};

VS_Sortie MiniPhongVS(float4 Pos : POSITION, float3 Normale : NORMAL, float2 coordTex : TEXCOORD)
{
	VS_Sortie sortie = (VS_Sortie)0;
	sortie.Pos_init = Pos;
	sortie.Pos = mul(Pos, matWorldViewProj);
	sortie.Norm = mul(float4(Normale, 0.0f), matWorld).xyz;

	float3 PosWorld = mul(Pos, matWorld).xyz;

	sortie.vDirLum = vLumiere[0].xyz - PosWorld;
	sortie.vDirCam = vCamera.xyz - PosWorld;

	// Coordonnées d'application de texture
	sortie.coordTex = coordTex;

	sortie.PosLumPt1 = vLumiere[1].xyz;
	sortie.PosLumPt2 = vLumiere[2].xyz;
	sortie.PosLumPt3 = vLumiere[3].xyz;

	sortie.vDirLumPt1 = vLumiere[1].xyz - PosWorld;
	sortie.vDirLumPt2 = vLumiere[2].xyz - PosWorld;
	sortie.vDirLumPt3 = vLumiere[3].xyz - PosWorld;

	sortie.intensityLum1 = vLumiere[1].w;
	sortie.intensityLum2 = vLumiere[2].w;
	sortie.intensityLum3 = vLumiere[3].w;

	sortie.distL1 = distance(sortie.PosLumPt1, sortie.Pos.xyz);
	sortie.distL2 = distance(sortie.PosLumPt2, sortie.Pos.xyz);
	sortie.distL3 = distance(sortie.PosLumPt3, sortie.Pos.xyz);

	return sortie;
}

Texture2D textureEntrees[8];  // la texture
Texture2D filter; // le filtre 
SamplerState SampleState;  // l'état de sampling

float4 MiniPhongPS(VS_Sortie vs) : SV_Target
{
	float3 couleur;

	// Normaliser les paramètres
	float3 N = normalize(vs.Norm);
	float3 L0 = normalize(vs.vDirLum);
	float3 L1 = normalize(vs.vDirLumPt1) * (1 / vs.distL1) * vs.intensityLum1;
	float3 L2 = normalize(vs.vDirLumPt2) * (1 / vs.distL2) * vs.intensityLum2;
	float3 L3 = normalize(vs.vDirLumPt3) * (1 / vs.distL3) * vs.intensityLum3;
	float3 V = normalize(vs.vDirCam) / 1.2;



	// Valeur de la composante diffuse
	float3 diff = saturate(dot(N, L0) *mainLightInfluence
						+ dot(N, L1) * pointLightInfluence
						+ dot(N, L2) * pointLightInfluence
						+ dot(N, L3) * pointLightInfluence);


	// vecReflexion = 2 * (N.L) * N – L
	float3 vecReflexion = normalize(2 * diff * N - L0);

	// Speculaire = (R · V)^p
	float Speculaire = pow(saturate(dot(vecReflexion, V)), 4.0f);

	// Échantillonner la couleur du pixel à partir de la texture
	float3 couleurTexture = textureEntrees[0].Sample(SampleState, vs.coordTex * 12).rgb;

	// I =     A + D * N.L + (R.V)n
	couleur = couleurTexture * vAEcl.rgb * vAMat.rgb + couleurTexture * vDEcl.rgb * vDMat.rgb * diff;

	couleur += Speculaire;


	return float4(couleur, 1.0f);
}

technique11 MiniPhong
{
	pass pass0
	{
		SetVertexShader(CompileShader(vs_5_0, MiniPhongVS()));
		SetPixelShader(CompileShader(ps_5_0, MiniPhongPS()));
		SetGeometryShader(NULL);
	}
}
