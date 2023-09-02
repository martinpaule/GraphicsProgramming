Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer ScreenSizeBuffer : register(b0)
{
    float screenSizeX;
    float time;
    float wobbleStrength;
    float wobbleSpeed;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
    float4 colour = float4(0.0f, 0.0f, 0.0f, 1.0f);

    //calculate texel size
    float widthTexelSize = 1.0f / screenSizeX;
    
    //calculate colour source offset
    float texelMove;
    texelMove = sin(input.tex.y*10 + time * wobbleSpeed);
    
    //get the colour from there
    colour = shaderTexture.Sample(SampleType, input.tex + float2(texelMove * widthTexelSize* wobbleStrength, 0.0f));
    

    
    saturate(colour);
	// Set the alpha channel to one.
    colour.a = 1.0f;

    return colour;
}
