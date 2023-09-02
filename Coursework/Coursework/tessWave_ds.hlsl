// Tessellation domain shader
// After tessellation the domain shader processes the all the vertices
//Texture2D texture0 : register(t1);
//SamplerState sampler0 : register(s1);

cbuffer MatrixBuffer : register(b0)
{
    matrix worldMatrix;
    matrix viewMatrix;
    matrix projectionMatrix;
};

cbuffer ManipulationBuffer : register(b1)
{
    float time;
    float amplitude;
    float frequency;
    float speed;
};


struct ConstantOutputType
{
    float edges[3] : SV_TessFactor;
    float inside : SV_InsideTessFactor;
    
};

struct InputType
{
    float4 position : POSITION;
    //float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

struct OutputType
{
    float4 position : SV_POSITION;
    float4 colour : COLOR;
    float3 normal : NORMAL;
};

[domain("tri")]
OutputType main(ConstantOutputType input, float3 uvwCoord : SV_DomainLocation, const OutputPatch<InputType, 4> patch)
{


    float3 vertexPosition;
    OutputType output;
    output.colour = float4(0.0f, 0.0f, 1.0f, 0.5f);

    // Determine the position of the new vertex.
    // Invert the y and Z components of uvwCoord as these coords are generated in UV space and therefore y is positive downward.
    // Alternatively you can set the output topology of the hull shader to cw instead of ccw (or vice versa).
   // vertexPosition = uvwCoord.x * patch[0].position + -uvwCoord.y * patch[1].position + -uvwCoord.z * patch[2].position;
    float3 v1 = lerp(patch[0].position, patch[1].position, uvwCoord.y);
    float3 v2 = lerp(patch[3].position, patch[2].position, uvwCoord.y);
    
    vertexPosition = lerp(v1, v2, uvwCoord.x);
    
    
    //raise the created vertex by a sin wave function
    float heightOffset = sin(vertexPosition.z * frequency + speed * time) * (amplitude);
    vertexPosition.y += heightOffset;

    //normal of a sine wave calculation by SPuntte on https://gamedev.net/forums/topic/551569-finding-the-normal-of-a-point-on-a-sine-wave/4548555/
    float tangentAngle = atan(heightOffset);
    float3 normal_ = float3(0.0f, cos(tangentAngle), -sin(tangentAngle));

    //calculate colour
    output.colour.r = (heightOffset + 1.0f) / 2.0f - 0.3f;
    output.colour.g = (heightOffset + 1.0f) / 2.0f - 0.3f;

    output.normal = normal_;
   
    // Calculate the position of the new vertex against the world, view, and projection matrices.
    output.position = mul(float4(vertexPosition, 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);
    

    
    return output;
}

