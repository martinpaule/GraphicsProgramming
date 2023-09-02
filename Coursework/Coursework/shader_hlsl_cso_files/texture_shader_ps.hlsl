// Texture pixel/fragment shader
// Basic fragment shader for rendering textured geometry

// Texture and sampler registers
Texture2D texture0 : register(t0);
SamplerState Sampler0 : register(s0);

cbuffer lightBuffer : register(b2)
{
    float4 diffuse;
    float3 direction;
    float normalDisp;
};

struct InputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};


float4 main(InputType input) : SV_TARGET
{
    if (normalDisp) {//used for debugging purposes
        return float4(input.normal, 1.0f);
    }
    else {
         return texture0.Sample(Sampler0, input.tex);
    }
}