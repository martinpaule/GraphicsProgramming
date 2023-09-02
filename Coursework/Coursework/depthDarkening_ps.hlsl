Texture2D shaderTexture : register(t0);
SamplerState SampleType : register(s0);

cbuffer ScreenSizeBuffer : register(b0)
{
    float camDepth;
    float3 padding;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

float4 main(InputType input) : SV_TARGET
{
    float4 colour = shaderTexture.Sample(SampleType, input.tex);
    //if ((input.tex.x < 0.25f && input.tex.x > 0.2f))
    //{
    //    colour = float4(0.0f, 0.0f, 0.0f, 1.0f);
    //}
    

    if (camDepth < -3)
    {
        colour /= (1+abs(camDepth)/20);
    }
    colour.a = 1.0f;
    return colour;
}
