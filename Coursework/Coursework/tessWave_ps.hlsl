// Tessellation pixel shader
// Output colour passed to stage.

struct InputType
{
    float4 position : SV_POSITION;
    float4 colour : COLOR;
    float3 normal : NORMAL;
};

cbuffer lightBuffer : register(b2)
{
    float4 diffuse;
    float3 direction;
    float normalDisp;
};

float4 main(InputType input) : SV_TARGET
{

    if (normalDisp) {
        return float4(input.normal, 1.0f);
    }
    else {
          return input.colour;
    }
}