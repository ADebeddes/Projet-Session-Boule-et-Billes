//---------------------------------------------------------------------
// Post Processing : apply visual effect on the final scene result 
//---------------------------------------------------------------------
#define EPSILON 0.000011


Texture2D textureEntree; // les textures
SamplerState SampleState; // l'etat de sampling
float randomNB;


// blur variables
float distance;
int nbIteration = 5;
float colorPourcent = 0.7;

//lens distortion variables
float power = -0.8f;

//activation des shaders : depuis les menus
bool shaderNull;
bool shaderRadialBlur;
bool shaderCroquis;
bool shaderBlackNWhite;

//-------------------------------------------------------
// default Vertex Shader : do nothing
//-------------------------------------------------------
struct VS_Sortie
{
	float4 Pos : SV_POSITION;
	float2 CoordTex : TEXCOORD0;
};

VS_Sortie NulVS(float4 Pos : POSITION, float2 CoordTex : TEXCOORD)
{
	VS_Sortie sortie = (VS_Sortie)0;

	sortie.Pos = Pos;
	sortie.CoordTex = CoordTex;

	return sortie;
}

//-----------------------------------------------------
// default Pixel Shader : do nothing
// technique number : 0
//-----------------------------------------------------
float4 NulPS(VS_Sortie vs) : SV_Target
{
	float4 couleur;

	couleur = textureEntree.Sample(SampleState, vs.CoordTex);

    /*if (int(randomNB * 10) == 3) {
        couleur = float4(1.0f, 1.0f, 1.0f, 1.0f);
    }*/
    //couleur = float4(randomNB, 0.0f, 0.0f, 1.0f);

    return couleur;
}

//-----------------------------------------------------
// Radial Blur
// technique number : 1
//-----------------------------------------------------
float4 RadialBlurPS(VS_Sortie vs) : SV_Target
{
	float4 couleur = 0;
    float4 ct;
    float2 tc = vs.CoordTex;
    float d, dx, dy;
    float x = tc.x * 2 - 1.0;
    float y = tc.y * 2 - 1.0;
    dx = sqrt(x * x);
    dy = sqrt(y * y);

    dx = x * (distance * dx);
    dy = y * (distance * dy);

    x = x - (dx * 10);
    y = y - (dy * 10);

    tc.x = (x + 1.0) / 2.0;
    tc.y = (y + 1.0) / 2.0;

    for (int i = 0; i < 10; i++)
    {
        ct = textureEntree.Sample(SampleState, tc);
        couleur = couleur * colorPourcent + ct * (1 - colorPourcent);

        x = x + dx;
        y = y + dy;

        tc.x = (x + 1.0) / 2.0;
        tc.y = (y + 1.0) / 2.0;
    }
	
    return couleur;
}

//-----------------------------------------------------
// croquis shader
// technique number : 2
//-----------------------------------------------------
float4 CroquisPS(VS_Sortie vs) : SV_Target
{
    float4 couleur;

    couleur = textureEntree.Sample(SampleState, vs.CoordTex);
    
    couleur -= textureEntree.Sample(SampleState, vs.CoordTex - 0.003) * 2.7f;
    couleur += textureEntree.Sample(SampleState, vs.CoordTex + 0.003) * 2.7f;
    
    couleur.rgb = (couleur.r + couleur.g + couleur.b) / 3.0f;
    
    return couleur; 
}

//-----------------------------------------------------
// full black and white shader
// technique number : 3
//-----------------------------------------------------
float4 BNWhitePS(VS_Sortie vs) : SV_Target
{
    float4 couleur;


    //black and white
    couleur = textureEntree.Sample(SampleState, vs.CoordTex);
    couleur.rgb = (couleur.r + couleur.g + couleur.b) / 3.0f;

    //red
    if (couleur.r < 0.2 || couleur.r > 0.9)
        couleur.r = 0;
    else
        couleur.r = 1.0f;

    //green
    if (couleur.g < 0.2 || couleur.g > 0.9)
        couleur.g = 0;
    else
        couleur.g = 1.0f;

    //blue
    if (couleur.b < 0.2 || couleur.b > 0.9)
        couleur.b = 0;
    else
        couleur.b = 1.0f;

    return couleur;
}

float2 generateFlake() {
    return(float2(randomNB, 1.0f));

}


//-----------------------------------------------------
// Compilation
//-----------------------------------------------------
technique11 Nul
{
	pass p0
	{
		VertexShader = compile vs_5_0 NulVS();
		PixelShader = compile ps_5_0 NulPS();
		SetGeometryShader(NULL);
	}
}

technique11 RadialBlur
{
	pass p0
	{
		VertexShader = compile vs_5_0 NulVS();
		PixelShader = compile ps_5_0 RadialBlurPS();
		SetGeometryShader(NULL);
	}
}

technique11 Croquis
{
    pass p0
    {
        VertexShader = compile vs_5_0 NulVS();
        PixelShader = compile ps_5_0 CroquisPS();
        SetGeometryShader(NULL);
    }
}

technique11 BNWhite
{
    pass p0
    {
        VertexShader = compile vs_5_0 NulVS();
        PixelShader = compile ps_5_0 BNWhitePS();
        SetGeometryShader(NULL);
    }
}


// work in progress
//-----------------------------------------------------
// FishEye, Lens Distortion
//-----------------------------------------------------
//// should be screen proportion
//   float screenProp = vs.CoordTex.x / vs.CoordTex.y;
//   float2 screenCenter = float2(0.5, 0.5 / screenProp);

//   float2 currCoord = vs.CoordTex - screenCenter;

//   float deltaRay = sqrt(dot(currCoord, currCoord));

//   float bind;
//   if (power > 0.0)
//       bind = sqrt(dot(screenCenter, screenCenter)); //stick to corners
//   else
//   {
//       if (screenProp < 1.0)
//           bind = screenCenter.x;
//       else
//           bind = screenCenter.y;
//   } //stick to borders

//   float2 uv;
//   if (power > 0.0)//fisheye
//       uv = screenCenter + normalize(currCoord) * tan(deltaRay * power) * bind / tan(bind * power);
//   else if (power < 0.0)//antifisheye
//       uv = screenCenter + normalize(currCoord) * atan(deltaRay * -power * 10.0) * bind / atan(-power * bind * 10.0);
//   else
//       uv = vs.CoordTex; //no effect

//   //vec3 col = texture2D( iChannel0, vec2(uv.x, -uv.y * prop)).xyz;
//   couleur = textureEntree.Sample(SampleState, float2(uv.x, uv.y * screenProp)); 